/**
 * Copyright (C) 2015-2016 Mavicona, Quenza Inc.
 * All Rights Reserved
 *
 * This file is part of the Mavicona project.
 *
 * Content can not be copied and/or distributed without the express
 * permission of the author.
 */

typedef int wby__check_ptr_size[(sizeof(void*) == sizeof(WBY_UINT_PTR)) ? 1 : -1];
#define WBY_LEN(a) (sizeof(a)/sizeof((a)[0]))
#define WBY_UNUSED(a) ((void)(a))

#ifdef WBY_USE_ASSERT
#ifndef WBY_ASSERT
#include <assert.h>
#define WBY_ASSERT(expr) assert(expr)
#endif
#else
#define WBY_ASSERT(expr)
#endif

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>

#define WBY_SOCK(s) ((wby_socket)(s))
#define WBY_INTERN static
#define WBY_GLOBAL static
#define WBY_STORAGE static

struct wby_buffer {
    wby_size used;
    /* current buffer size */
    wby_size max;
    /* buffer capacity */
    wby_byte *data;
    /* pointer inside a global buffer */
};

WBY_INTERN void
wby_dbg(wby_log_f log, const char *fmt, ...)
{
    char buffer[1024];
    va_list args;
    if (!log) return;

    va_start(args, fmt);
    vsnprintf(buffer, sizeof buffer, fmt, args);
    va_end(args);
    buffer[(sizeof buffer)-1] = '\0';
    log(buffer);
}

WBY_INTERN int
wb_peek_request_size(const wby_byte *buf, int len)
{
    int i;
    int max = len - 3;
    for (i = 0; i < max; ++i) {
        if ('\r' != buf[i + 0]) continue;
        if ('\n' != buf[i + 1]) continue;
        if ('\r' != buf[i + 2]) continue;
        if ('\n' != buf[i + 3]) continue;
        /* OK; we have CRLFCRLF which indicates the end of the header section */
        return i + 4;
    }
    return -1;
}

WBY_INTERN char*
wby_skipws(char *p)
{
    for (;;) {
        char ch = *p;
        if (' ' == ch || '\t' == ch) ++p;
        else break;
    }
    return p;
}

#define WBY_TOK_SKIPWS WBY_FLAG(0)
WBY_INTERN int
wby_tok_inplace(char *buf, const char* separator, char *tokens[], int max, int flags)
{
    char *b = buf;
    char *e = buf;
    int token_count = 0;
    int separator_len = (int)strlen(separator);
    while (token_count < max) {
        if (flags & WBY_TOK_SKIPWS)
            b = wby_skipws(b);
        if (NULL != (e = strstr(b, separator))) {
            int len = (int) (e - b);
            if (len > 0)
                tokens[token_count++] = b;
            *e = '\0';
            b = e + separator_len;
        } else {
            tokens[token_count++] = b;
            break;
        }
    }
    return token_count;
}

WBY_INTERN wby_size
wby_make_websocket_header(wby_byte buffer[10], wby_byte opcode,
    int payload_len, int fin)
{
    buffer[0] = (wby_byte)((fin ? 0x80 : 0x00) | opcode);
    if (payload_len < 126) {
        buffer[1] = (wby_byte)(payload_len & 0x7f);
        return 2;
    } else if (payload_len < 65536) {
        buffer[1] = 126;
        buffer[2] = (wby_byte)(payload_len >> 8);
        buffer[3] = (wby_byte)payload_len;
        return 4;
    } else {
        buffer[1] = 127;
        /* Ignore high 32-bits. I didn't want to require 64-bit types and typdef hell in the API. */
        buffer[2] = buffer[3] = buffer[4] = buffer[5] = 0;
        buffer[6] = (wby_byte)(payload_len >> 24);
        buffer[7] = (wby_byte)(payload_len >> 16);
        buffer[8] = (wby_byte)(payload_len >> 8);
        buffer[9] = (wby_byte)payload_len;
        return 10;
    }
}

WBY_INTERN int
wby_read_buffered_data(int *data_left, struct wby_buffer* buffer,
    char **dest_ptr, wby_size *dest_len)
{
    int offset, read_size;
    int left = *data_left;
    int len;
    if (left == 0)
        return 0;

    len = (int) *dest_len;
    offset = (int)buffer->used - left;
    read_size = (len > left) ? left : len;
    memcpy(*dest_ptr, buffer->data + offset, (wby_size)read_size);

    (*dest_ptr) += read_size;
    (*dest_len) -= (wby_size) read_size;
    (*data_left) -= read_size;
    return read_size;
}
