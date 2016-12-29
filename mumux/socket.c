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
#include "connection.h"
#include "util.h"
#include "socket.h"

#ifdef _WIN32

int wby_socket_error() {
    return WSAGetLastError();
}

#if !defined(__GNUC__)
int strcasecmp(const char *a, const char *b) {
    return _stricmp(a, b);
}

int strncasecmp(const char *a, const char *b, unsigned long len) {
    return _strnicmp(a, b, len);
}
#endif

int wby_socket_set_blocking(wby_socket socket, int blocking) {
    u_long val = !blocking;
    return ioctlsocket(socket, FIONBIO, &val);
}

int wby_socket_is_valid(wby_socket socket) {
    return (INVALID_SOCKET != socket);
}

void wby_socket_close(wby_socket socket) {
    closesocket(socket);
}

int wby_socket_is_blocking_error(int error) {
    return WSAEWOULDBLOCK == error;
}

#else /* UNIX */

#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <strings.h>

int wby_socket_error() {
    return errno;
}

int wby_socket_is_valid(wby_socket socket) {
    return (socket > 0);
}

void wby_socket_close(wby_socket socket) {
    close(socket);
}

int wby_socket_is_blocking_error(int error) {
    return (EAGAIN == error);
}

int wby_socket_set_blocking(wby_socket socket, int blocking) {
    int flags = fcntl(socket, F_GETFL, 0);
    if (flags < 0) return flags;
    flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
    return fcntl(socket, F_SETFL, flags);
}
#endif


int wby_socket_config_incoming(wby_socket socket) {
    wby_sockopt off = 0;
    int err;
    if ((err = wby_socket_set_blocking(socket, 0)) != WBY_OK) return err;
    setsockopt(socket, SOL_SOCKET, SO_LINGER, (const char*) &off, sizeof(int));
    return 0;
}

int wby_socket_send(wby_socket socket, const unsigned char *buffer, int size) {
    while (size > 0) {
        long err = send(socket, (const char*)buffer, (unsigned long)size, 0);
        if (err <= 0) return 1;
        buffer += err;
        size -= (int)err;
    }
    return 0;
}

int wby_socket_recv(wby_socket socket, struct wby_buffer *buf, wby_log_f log) {
    long err;
    int buf_left;
    for (;;) {
        buf_left = (int)buf->max - (int)buf->used;
        wby_dbg(log, "buffer space left = %d", buf_left);
        if (buf_left == 0)
            return WBY_FILL_FULL;

        /* Read what we can into the current buffer space. */
        err = recv(socket, (char*)buf->data + buf->used, (unsigned long)buf_left, 0);
        if (err < 0) {
            int sock_err = wby_socket_error();
            if (wby_socket_is_blocking_error(sock_err)) {
                return WBY_FILL_OK;
            } else {
                /* Read error. Give up. */
                wby_dbg(log, "read error %d - connection dead", sock_err);
                return WBY_FILL_ERROR;
            }
        } else if (err == 0) {
          /* The peer has closed the connection. */
          wby_dbg(log, "peer has closed the connection");
          return WBY_FILL_ERROR;
        } else buf->used += (unsigned long)err;
    }
}

int wby_socket_flush(wby_socket socket, struct wby_buffer *buf) {
    if (buf->used > 0){
        if (wby_socket_send(socket, buf->data, (int)buf->used) != WBY_OK)
            return 1;
    }
    buf->used = 0;
    return 0;
}
