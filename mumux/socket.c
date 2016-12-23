/**
 * Copyright (C) 2015-2016 Mavicona, Quenza Inc.
 * All Rights Reserved
 *
 * This file is part of the Mavicona project.
 *
 * Content can not be copied and/or distributed without the express
 * permission of the author.
 */

#ifdef _WIN32
#include <winsock2.h>
typedef SOCKET wby_socket;
typedef int wby_socklen;
typedef char wby_sockopt;

#if defined(__GNUC__)
#define WBY_ALIGN(x) __attribute__((aligned(x)))
#else
#define WBY_ALIGN(x) __declspec(align(x))
#endif

#define WBY_INVALID_SOCKET INVALID_SOCKET
#define snprintf _snprintf

WBY_INTERN int
wby_socket_error(void)
{
    return WSAGetLastError();
}

#if !defined(__GNUC__)
WBY_INTERN int
strcasecmp(const char *a, const char *b)
{
    return _stricmp(a, b);
}

WBY_INTERN int
strncasecmp(const char *a, const char *b, wby_size len)
{
    return _strnicmp(a, b, len);
}
#endif

WBY_INTERN int
wby_socket_set_blocking(wby_socket socket, int blocking)
{
    u_long val = !blocking;
    return ioctlsocket(socket, FIONBIO, &val);
}

WBY_INTERN int
wby_socket_is_valid(wby_socket socket)
{
    return (INVALID_SOCKET != socket);
}

WBY_INTERN void
wby_socket_close(wby_socket socket)
{
    closesocket(socket);
}

WBY_INTERN int
wby_socket_is_blocking_error(int error)
{
    return WSAEWOULDBLOCK == error;
}

#else /* UNIX */

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <strings.h>

typedef int wby_socket;
typedef socklen_t wby_socklen;
typedef int wby_sockopt;

#define WBY_ALIGN(x) __attribute__((aligned(x)))
#define WBY_INVALID_SOCKET (-1)

WBY_INTERN int
wby_socket_error(void)
{
    return errno;
}

WBY_INTERN int
wby_socket_is_valid(wby_socket socket)
{
    return (socket > 0);
}

WBY_INTERN void
wby_socket_close(wby_socket socket)
{
    close(socket);
}

WBY_INTERN int
wby_socket_is_blocking_error(int error)
{
    return (EAGAIN == error);
}

WBY_INTERN int
wby_socket_set_blocking(wby_socket socket, int blocking)
{
    int flags = fcntl(socket, F_GETFL, 0);
    if (flags < 0) return flags;
    flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
    return fcntl(socket, F_SETFL, flags);
}
#endif

WBY_INTERN int
wby_socket_config_incoming(wby_socket socket)
{
    wby_sockopt off = 0;
    int err;
    if ((err = wby_socket_set_blocking(socket, 0)) != WBY_OK) return err;
    setsockopt(socket, SOL_SOCKET, SO_LINGER, (const char*) &off, sizeof(int));
    return 0;
}

WBY_INTERN int
wby_socket_send(wby_socket socket, const wby_byte *buffer, int size)
{
    while (size > 0) {
        long err = send(socket, (const char*)buffer, (wby_size)size, 0);
        if (err <= 0) return 1;
        buffer += err;
        size -= (int)err;
    }
    return 0;
}

/* Read as much as possible without blocking while there is buffer space. */
enum {WBY_FILL_OK, WBY_FILL_ERROR, WBY_FILL_FULL};
WBY_INTERN int
wby_socket_recv(wby_socket socket, struct wby_buffer *buf, wby_log_f log)
{
    long err;
    int buf_left;
    for (;;) {
        buf_left = (int)buf->max - (int)buf->used;
        wby_dbg(log, "buffer space left = %d", buf_left);
        if (buf_left == 0)
            return WBY_FILL_FULL;

        /* Read what we can into the current buffer space. */
        err = recv(socket, (char*)buf->data + buf->used, (wby_size)buf_left, 0);
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
        } else buf->used += (wby_size)err;
    }
}

WBY_INTERN int
wby_socket_flush(wby_socket socket, struct wby_buffer *buf)
{
    if (buf->used > 0){
        if (wby_socket_send(socket, buf->data, (int)buf->used) != WBY_OK)
            return 1;
    }
    buf->used = 0;
    return 0;
}
