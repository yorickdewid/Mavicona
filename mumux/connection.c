/**
 * Copyright (C) 2015-2016 Mavicona, Quenza Inc.
 * All Rights Reserved
 *
 * This file is part of the Mavicona project.
 *
 * Content can not be copied and/or distributed without the express
 * permission of the author.
 */

#include "internal.h"
#include "socket.h"
#include "connection.h"
#include "url.h"
#include "sha.h"
#include "base64.h"

#include <stdlib.h>
#include <strings.h>
#include <memory.h>

int wby_connection_set_blocking(struct wby_connection *conn) {
    if (conn->blocking_count == 0) {
        if (wby_socket_set_blocking(WBY_SOCK(conn->socket), 1) != WBY_OK) {
            wby_dbg(conn->log, "failed to switch connection to blocking");
            conn->flags &= (unsigned short)~WBY_CON_FLAG_ALIVE;
            return -1;
        }
    }
    ++conn->blocking_count;
    return 0;
}

int wby_connection_set_nonblocking(struct wby_connection *conn) {
    unsigned long count = conn->blocking_count;
    if ((conn->flags & WBY_CON_FLAG_ALIVE) != 0 && count == 1) {
        if (wby_socket_set_blocking(WBY_SOCK(conn->socket), 0) != WBY_OK) {
            wby_dbg(conn->log, "failed to switch connection to non-blocking");
            conn->flags &= (unsigned short)~WBY_CON_FLAG_ALIVE;
            return -1;
        }
    }
    conn->blocking_count = count - 1;
    return 0;
}

void wby_connection_reset(struct wby_connection *conn, unsigned long request_buffer_size,
    unsigned long io_buffer_size) {
    conn->header_buf.used = 0;
    conn->header_buf.max = request_buffer_size;
    conn->io_buf.used = 0;
    conn->io_buf.max = io_buffer_size;
    conn->header_body_left = 0;
    conn->io_data_left = 0;
    conn->continue_data_left = 0;
    conn->body_bytes_read = 0;
    conn->state = WBY_CON_STATE_REQUEST;
    conn->public_data.user_data = NULL;
    conn->blocking_count = 0;
}

void wby_connection_close(struct wby_connection* connection) {
    if (WBY_SOCK(connection->socket) != WBY_INVALID_SOCKET) {
        wby_socket_close(WBY_SOCK(connection->socket));
        connection->socket = (unsigned long)WBY_INVALID_SOCKET;
    }
    connection->flags = 0;
}

int wby_connection_setup_request(struct wby_connection *connection, int request_size) {
    char* lines[WBY_MAX_HEADERS + 2];
    int line_count;
    char* tok[16];
    char* query_params;
    int tok_count;

    int i;
    int header_count;
    char *buf = (char*) connection->header_buf.data;
    struct wby_request *req = &connection->public_data.request;

    /* Null-terminate the request envelope by overwriting the last CRLF with 00LF */
    buf[request_size - 2] = '\0';
    /* Split header into lines */
    line_count = wby_tok_inplace(buf, "\r\n", lines, WBY_LEN(lines), 0);
    header_count = line_count - 2;
    if (line_count < 1 || header_count > (int) WBY_LEN(req->headers))
        return 1;

    /* Parse request line */
    tok_count = wby_tok_inplace(lines[0], " ", tok, WBY_LEN(tok), 0);
    if (3 != tok_count) return 1;

    req->method = tok[0];
    req->uri = tok[1];
    req->http_version = tok[2];
    req->content_length = 0;

    /* See if there are any query parameters */
    if ((query_params = (char*) strchr(req->uri, '?')) != NULL) {
        req->query_params = query_params + 1;
        *query_params = '\0';
    } else req->query_params = NULL;

    {
        /* Decode the URI in place */
        unsigned long uri_len = strlen(req->uri);
        wby_url_decode(req->uri, uri_len, (char*)req->uri, uri_len + 1, 1);
    }

    /* Parse headers */
    for (i = 0; i < header_count; ++i) {
        tok_count = wby_tok_inplace(lines[i + 1], ":", tok, 2, WBY_TOK_SKIPWS);
        if (tok_count != 2) return 1;
        req->headers[i].name = tok[0];
        req->headers[i].value = tok[1];

        if (!strcasecmp("content-length", tok[0])) {
            req->content_length = (int)strtoul(tok[1], NULL, 10);
            wby_dbg(connection->log, "request has body; content length is %d", req->content_length);
        } else if (!strcasecmp("transfer-encoding", tok[0])) {
            wby_dbg(connection->log, "cowardly refusing to handle Transfer-Encoding: %s", tok[1]);
            return 1;
        }
    }
    req->header_count = header_count;
    return 0;
}

int wby_connection_send_websocket_upgrade(struct wby_connection* connection) {
    const char *hdr;
    struct wby_sha1 sha;
    unsigned char digest[20];
    char output_digest[64];
    struct wby_header headers[3];
    struct wby_con *conn = &connection->public_data;
    if ((hdr = wby_find_header(conn, "Sec-WebSocket-Version")) == NULL) {
        wby_dbg(connection->log, "Sec-WebSocket-Version header not present");
        return 1;
    }
    if (strcmp(hdr, WBY_WEBSOCKET_VERSION)) {
        wby_dbg(connection->log,"WebSocket version %s not supported (we only do %s)",
                hdr, WBY_WEBSOCKET_VERSION);
        return 1;
    }
    if ((hdr = wby_find_header(conn, "Sec-WebSocket-Key")) == NULL) {
        wby_dbg(connection->log, "Sec-WebSocket-Key header not present");
        return 1;
    }
    /* Compute SHA1 hash of Sec-Websocket-Key + the websocket guid as required by
    * the RFC.
    *
    * This handshake is bullshit. It adds zero security. Just forces me to drag
    * in SHA1 and create a base64 encoder.
    */
    wby_sha1_init(&sha);
    wby_sha1_update(&sha, hdr, strlen(hdr));
    wby_sha1_update(&sha, wby_websocket_guid, wby_websocket_guid_len);
    wby_sha1_final(&digest[0], &sha);
    if (wby_base64_encode(output_digest, sizeof output_digest, &digest[0], sizeof(digest)) != WBY_OK)
        return 1;

    headers[0].name  = "Upgrade";
    headers[0].value = "websocket";
    headers[1].name  = "Connection";
    headers[1].value = "Upgrade";
    headers[2].name  = "Sec-WebSocket-Accept";
    headers[2].value = output_digest;
    wby_response_begin(&connection->public_data, 101, 0, headers, WBY_LEN(headers));
    wby_response_end(&connection->public_data);
    return 0;
}

int wby_connection_push(struct wby_connection *conn, const void *data_, int len) {
    struct wby_buffer *buf = &conn->io_buf;
    const unsigned char* data = (const unsigned char*)data_;
    if (conn->state != WBY_CON_STATE_SERVE) {
        wby_dbg(conn->log, "attempt to write in non-serve state");
        return 1;
    }
    if (len == 0)
        return wby_socket_flush(WBY_SOCK(conn->socket), buf);

    while (len > 0) {
        int buf_space = (int)buf->max - (int)buf->used;
        int copy_size = len < buf_space ? len : buf_space;
        memcpy(buf->data + buf->used, data, (unsigned long)copy_size);
        buf->used += (unsigned long)copy_size;

        data += copy_size;
        len -= copy_size;
        if (buf->used == buf->max) {
            if (wby_socket_flush(WBY_SOCK(conn->socket), buf) != WBY_OK)
                return 1;
            if ((unsigned long)len >= buf->max)
                return wby_socket_send(WBY_SOCK(conn->socket), data, len);
        }
    }
    return 0;
}
