/**
 * Copyright (C) 2015-2016 Mavicona, Quenza Inc.
 * All Rights Reserved
 *
 * This file is part of the Mavicona project.
 *
 * Content can not be copied and/or distributed without the express
 * permission of the author.
 */

#ifndef MUMUX_CONNECTION_H
#define MUMUX_CONNECTION_H

#include "util.h"

#define WBY_WEBSOCKET_VERSION "13"
static const char wby_continue_header[] = "HTTP/1.1 100 Continue\r\n\r\n";
static const unsigned long wby_continue_header_len = sizeof(wby_continue_header) - 1;
static const char wby_websocket_guid[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
static const unsigned long wby_websocket_guid_len = sizeof(wby_websocket_guid) - 1;
static const unsigned char wby_websocket_pong[] = { 0x80, WBY_WSOP_PONG, 0x00 };
static const struct wby_header wby_plain_text_headers[]={{"Content-Type","text/plain"}};

enum wby_connection_flags {
    WBY_CON_FLAG_ALIVE                  = WBY_FLAG(0),
    WBY_CON_FLAG_FRESH_CONNECTION       = WBY_FLAG(1),
    WBY_CON_FLAG_CLOSE_AFTER_RESPONSE   = WBY_FLAG(2),
    WBY_CON_FLAG_CHUNKED_RESPONSE       = WBY_FLAG(3),
    WBY_CON_FLAG_WEBSOCKET              = WBY_FLAG(4)
};

enum wby_connection_state {
    WBY_CON_STATE_REQUEST,
    WBY_CON_STATE_SEND_CONTINUE,
    WBY_CON_STATE_SERVE,
    WBY_CON_STATE_WEBSOCKET
};

struct wby_connection {
    struct wby_con public_data;
    unsigned short flags;
    unsigned short state;

    unsigned long socket;
    wby_log_f log;

    unsigned long request_buffer_size;
    struct wby_buffer header_buf;
    struct wby_buffer io_buf;
    unsigned long io_buffer_size;

    int header_body_left;
    int io_data_left;
    int continue_data_left;
    int body_bytes_read;

    struct wby_frame ws_frame;
    unsigned char ws_opcode;
    unsigned long blocking_count;
};

int wby_connection_set_blocking(struct wby_connection *conn);
int wby_connection_set_nonblocking(struct wby_connection *conn);
void wby_connection_reset(struct wby_connection *conn, unsigned long request_buffer_size,
    unsigned long io_buffer_size);
void wby_connection_close(struct wby_connection* connection);
int wby_connection_setup_request(struct wby_connection *connection, int request_size);
int wby_connection_send_websocket_upgrade(struct wby_connection* connection);
int wby_connection_push(struct wby_connection *conn, const void *data_, int len);

#endif // MUMUX_CONNECTION_H
