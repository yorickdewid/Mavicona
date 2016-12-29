/**
 * Copyright (C) 2015-2016 Mavicona, Quenza Inc.
 * All Rights Reserved
 *
 * This file is part of the Mavicona project.
 *
 * Content can not be copied and/or distributed without the express
 * permission of the author.
 */

#ifndef MUMUX_REQUEST_H
#define MUMUX_REQUEST_H

int wby_con_discard_incoming_data(struct wby_con* conn, int count);
const char *wby_find_header(struct wby_con *conn, const char *name);
int wby_con_is_websocket_request(struct wby_con* conn);
int wby_scan_websocket_frame(struct wby_frame *frame, const struct wby_buffer *buf);
int wby_frame_begin(struct wby_con *conn_pub, int opcode);
int wby_frame_end(struct wby_con *conn_pub);

int wby_printf(struct wby_con* conn, const char* fmt, ...);

#endif // MUMUX_REQUEST_H
