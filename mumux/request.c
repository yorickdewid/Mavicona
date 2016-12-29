#include "internal.h"
#include "socket.h"
#include "connection.h"

#include <stdio.h>
#include <stdarg.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>

int wby_con_discard_incoming_data(struct wby_con* conn, int count) {
    while (count > 0) {
        char buffer[1024];
        int read_size = (int)(((unsigned long)count > sizeof(buffer)) ?
            sizeof(buffer) : (unsigned long)count);
        if (wby_read(conn, buffer, (unsigned long)read_size) != WBY_OK)
            return -1;
        count -= read_size;
    }
    return 0;
}

const char *wby_find_header(struct wby_con *conn, const char *name) {
    int i, count;
    for (i = 0, count = conn->request.header_count; i < count; ++i) {
        if (!strcasecmp(conn->request.headers[i].name, name))
            return conn->request.headers[i].value;
    }
    return NULL;
}

int wby_con_is_websocket_request(struct wby_con* conn) {
    const char *hdr;
    if ((hdr = wby_find_header(conn, "Connection")) == NULL) return 0;
    if (strcasecmp(hdr, "Upgrade")) return 0;
    if ((hdr = wby_find_header(conn, "Upgrade")) == NULL) return 0;
    if (strcasecmp(hdr, "websocket")) return 0;
    return 1;
}

int wby_scan_websocket_frame(struct wby_frame *frame, const struct wby_buffer *buf) {
    unsigned char flags = 0;
    unsigned int len = 0;
    unsigned int opcode = 0;
    unsigned char* data = buf->data;
    unsigned char* data_max = data + buf->used;

    int i;
    int len_bytes = 0;
    int mask_bytes = 0;
    unsigned char header0, header1;
    if (buf->used < 2)
        return -1;

    header0 = *data++;
    header1 = *data++;
    if (header0 & 0x80)
        flags |= WBY_WSF_FIN;
    if (header1 & 0x80) {
        flags |= WBY_WSF_MASKED;
        mask_bytes = 4;
    }

    opcode = header0 & 0xf;
    len = header1 & 0x7f;
    if (len == 126)
        len_bytes = 2;
    else if (len == 127)
        len_bytes = 8;
    if (data + len_bytes + mask_bytes > data_max)
        return -1;

    /* Read big endian length from length bytes (if greater than 125) */
    len = len_bytes == 0 ? len : 0;
    for (i = 0; i < len_bytes; ++i) {
        /* This will totally overflow for 64-bit values. I don't care.
         * If you're transmitting more than 4 GB of data using Webby,
         * seek help. */
        len <<= 8;
        len |= *data++;
    }

    /* Read mask word if present */
    for (i = 0; i < mask_bytes; ++i)
        frame->mask_key[i] = *data++;
    frame->header_size = (unsigned char) (data - buf->data);
    frame->flags = flags;
    frame->opcode = (unsigned char) opcode;
    frame->payload_length = (int)len;
    return 0;
}

int wby_frame_begin(struct wby_con *conn_pub, int opcode) {
    struct wby_connection *conn = (struct wby_connection*)conn_pub;
    conn->ws_opcode = (unsigned char) opcode;
    /* Switch socket to blocking mode */
    return wby_connection_set_blocking(conn);
}

int wby_frame_end(struct wby_con *conn_pub) {
    unsigned char header[10];
    unsigned long header_size;
    struct wby_connection *conn = (struct wby_connection*) conn_pub;
    header_size = wby_make_websocket_header(header, conn->ws_opcode, 0, 1);
    if (wby_socket_send(WBY_SOCK(conn->socket), header, (int) header_size) != WBY_OK)
        conn->flags &= (unsigned short)~WBY_CON_FLAG_ALIVE;
    /* Switch socket to non-blocking mode */
    return wby_connection_set_nonblocking(conn);
}

int wby_read(struct wby_con *conn, void *ptr_, unsigned long len) {
    struct wby_connection* conn_prv = (struct wby_connection*)conn;
    char *ptr = (char*) ptr_;
    int count;

    int start_pos = conn_prv->body_bytes_read;
    if (conn_prv->header_body_left > 0) {
        count = wby_read_buffered_data(&conn_prv->header_body_left, &conn_prv->header_buf, &ptr, &len);
        conn_prv->body_bytes_read += count;
    }

    /* Read buffered websocket data */
    if (conn_prv->io_data_left > 0) {
        count = wby_read_buffered_data(&conn_prv->io_data_left, &conn_prv->io_buf, &ptr, &len);
        conn_prv->body_bytes_read += count;
    }

    while (len > 0) {
        long err = recv(WBY_SOCK(conn_prv->socket), ptr, (unsigned long)len, 0);
        if (err < 0) {
            conn_prv->flags &= (unsigned short)~WBY_CON_FLAG_ALIVE;
            return (int)err;
        }
        len -= (unsigned long)err;
        ptr += (unsigned long)err;
        conn_prv->body_bytes_read += (int)err;
    }

    if ((conn_prv->flags & WBY_CON_FLAG_WEBSOCKET) && (conn_prv->ws_frame.flags & WBY_WSF_MASKED)) {
        /* XOR outgoing data with websocket ofuscation key */
        int i, end_pos = conn_prv->body_bytes_read;
        const unsigned char *mask = conn_prv->ws_frame.mask_key;
        ptr = (char*) ptr_; /* start over */
        for (i = start_pos; i < end_pos; ++i) {
            unsigned char byte = (unsigned char)*ptr;
            *ptr++ = (char)(byte ^ mask[i & 3]);
        }
    }
    return 0;
}

int wby_write(struct wby_con *conn, const void *ptr, unsigned long len) {
    struct wby_connection *conn_priv = (struct wby_connection*) conn;
    if (conn_priv->flags & WBY_CON_FLAG_WEBSOCKET) {
        unsigned char header[10];
        unsigned long header_size;
        header_size = wby_make_websocket_header(header, conn_priv->ws_opcode, (int)len, 0);

        /* Overwrite opcode to be continuation packages from here on out */
        conn_priv->ws_opcode = WBY_WSOP_CONTINUATION;
        if (wby_socket_send(WBY_SOCK(conn_priv->socket), header, (int)header_size) != WBY_OK) {
            conn_priv->flags &= (unsigned short)~WBY_CON_FLAG_ALIVE;
            return -1;
        }
        if (wby_socket_send(WBY_SOCK(conn_priv->socket),(const unsigned char*)ptr, (int)len) != WBY_OK) {
            conn_priv->flags &= (unsigned short)~WBY_CON_FLAG_ALIVE;
            return -1;
        }
        return 0;
    } else if (conn_priv->flags & WBY_CON_FLAG_CHUNKED_RESPONSE) {
        char chunk_header[128];
        int header_len = snprintf(chunk_header, sizeof chunk_header, "%x\r\n", (int) len);
        wby_connection_push(conn_priv, chunk_header, header_len);
        wby_connection_push(conn_priv, ptr, (int)len);
        return wby_connection_push(conn_priv, "\r\n", 2);
    } else return wby_connection_push(conn_priv, ptr, (int) len);
}

int wby_printf(struct wby_con* conn, const char* fmt, ...) {
    int len;
    char buffer[1024];
    va_list args;
    va_start(args, fmt);
    len = vsnprintf(buffer, sizeof buffer, fmt, args);
    va_end(args);
    return wby_write(conn, buffer, (unsigned long)len);
}
