#include "internal.h"
#include "connection.h"
#include "request.h"

#include <strings.h>

#define WBY_STATUS_MAP(STATUS)\
    STATUS(100, "Continue")\
    STATUS(101, "Switching Protocols")\
    STATUS(200, "OK")\
    STATUS(201, "Created")\
    STATUS(202, "Accepted")\
    STATUS(203, "Non-Authoritative Information")\
    STATUS(204, "No Content")\
    STATUS(205, "Reset Content")\
    STATUS(206, "Partial Content")\
    STATUS(300, "Multiple Choices")\
    STATUS(301, "Moved Permanently")\
    STATUS(302, "Found")\
    STATUS(303, "See Other")\
    STATUS(304, "Not Modified")\
    STATUS(305, "Use Proxy")\
    STATUS(307, "Temporary Redirect")\
    STATUS(400, "Bad Request")\
    STATUS(401, "Unauthorized")\
    STATUS(402, "Payment Required")\
    STATUS(403, "Forbidden")\
    STATUS(404, "Not Found")\
    STATUS(405, "Method Not Allowed")\
    STATUS(406, "Not Acceptable")\
    STATUS(407, "Proxy Authentication Required")\
    STATUS(408, "Request Time-out")\
    STATUS(409, "Conflict")\
    STATUS(410, "Gone")\
    STATUS(411, "Length Required")\
    STATUS(412, "Precondition Failed")\
    STATUS(413, "Request Entity Too Large")\
    STATUS(414, "Request-URI Too Large")\
    STATUS(415, "Unsupported Media Type")\
    STATUS(416, "Requested range not satisfiable")\
    STATUS(417, "Expectation Failed")\
    STATUS(500, "Internal Server Error")\
    STATUS(501, "Not Implemented")\
    STATUS(502, "Bad Gateway")\
    STATUS(503, "Service Unavailable")\
    STATUS(504, "Gateway Time-out")\
    STATUS(505, "HTTP Version not supported")

WBY_GLOBAL const short wby_status_nums[] = {
#define WBY_STATUS(id, name) id,
    WBY_STATUS_MAP(WBY_STATUS)
#undef WBY_STATUS
};
WBY_GLOBAL const char* wby_status_text[] = {
#define WBY_STATUS(id, name) name,
    WBY_STATUS_MAP(WBY_STATUS)
#undef WBY_STATUS
};

const char *wby_response_status_text(int status_code) {
    int i;
    for (i = 0; i < (int) WBY_LEN(wby_status_nums); ++i) {
        if (wby_status_nums[i] == status_code)
            return wby_status_text[i];
    }
    return "Unknown";
}

int wby_response_begin(struct wby_con *conn_pub, int status_code, int content_length,
    const struct wby_header *headers, int header_count) {
    int i = 0;
    struct wby_connection *conn = (struct wby_connection *)conn_pub;
    if (conn->body_bytes_read < (int)conn->public_data.request.content_length) {
        int body_left = conn->public_data.request.content_length - (int)conn->body_bytes_read;
        if (wby_con_discard_incoming_data(conn_pub, body_left) != WBY_OK) {
            conn->flags &= (unsigned short)~WBY_CON_FLAG_ALIVE;
            return -1;
        }
    }

    wby_printf(conn_pub, "HTTP/1.1 %d %s\r\n", status_code, wby_response_status_text(status_code));
    if (content_length >= 0)
        wby_printf(conn_pub, "Content-Length: %d\r\n", content_length);
    else
        wby_printf(conn_pub, "Transfer-Encoding: chunked\r\n");
    wby_printf(conn_pub, "Server: wby\r\n");

    for (i = 0; i < header_count; ++i) {
        if (!strcasecmp(headers[i].name, "Connection")) {
            if (!strcasecmp(headers[i].value, "close"))
            conn->flags |= WBY_CON_FLAG_CLOSE_AFTER_RESPONSE;
        }
        wby_printf(conn_pub, "%s: %s\r\n", headers[i].name, headers[i].value);
    }

    if (!(conn->flags & WBY_CON_FLAG_CLOSE_AFTER_RESPONSE)) {
        /* See if the client wants us to close the connection. */
        const char* connection_header = wby_find_header(conn_pub, "Connection");
        if (connection_header && !strcasecmp("close", connection_header)) {
            conn->flags |= WBY_CON_FLAG_CLOSE_AFTER_RESPONSE;
            wby_printf(conn_pub, "Connection: close\r\n");
        }
    }

    wby_printf(conn_pub, "\r\n");
    if (content_length < 0)
        conn->flags |= WBY_CON_FLAG_CHUNKED_RESPONSE;

    return 0;
}

void wby_response_end(struct wby_con *conn) {
    struct wby_connection *conn_priv = (struct wby_connection*) conn;
    if (conn_priv->flags & WBY_CON_FLAG_CHUNKED_RESPONSE) {
        /* Write final chunk */
        wby_connection_push(conn_priv, "0\r\n\r\n", 5);
        conn_priv->flags &= (unsigned short)~WBY_CON_FLAG_CHUNKED_RESPONSE;
    }

    /* Flush buffers */
    wby_connection_push(conn_priv, "", 0);

    /* Close connection when Content-Length is zero that maybe HTTP/1.0. */
    if (conn->request.content_length == 0)
        wby_connection_close(conn_priv);
}
