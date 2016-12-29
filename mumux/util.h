#ifndef MUMUX_UTIL_H
#define MUMUX_UTIL_H

#define WBY_SOCK(s) ((wby_socket)(s))
#define WBY_INTERN static
#define WBY_GLOBAL static
#define WBY_STORAGE static

#define WBY_TOK_SKIPWS WBY_FLAG(0)

struct wby_buffer {
    unsigned long used;
    /* current buffer size */
    unsigned long max;
    /* buffer capacity */
    unsigned char *data;
    /* pointer inside a global buffer */
};

void wby_dbg(wby_log_f log, const char *fmt, ...);
int wb_peek_request_size(const unsigned char *buf, int len);
char *wby_skipws(char *p);
int wby_tok_inplace(char *buf, const char* separator, char *tokens[], int max, int flags);
unsigned long wby_make_websocket_header(unsigned char buffer[10], unsigned char opcode,
    int payload_len, int fin);
int wby_read_buffered_data(int *data_left, struct wby_buffer* buffer,
    char **dest_ptr, unsigned long *dest_len);

#endif // MUMUX_UTIL_H
