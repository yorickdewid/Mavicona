#ifndef MUMUX_SOCKET_H
#define MUMUX_SOCKET_H

#include "util.h"

#ifdef _WIN32

#include <winsock2.h>
typedef SOCKET wby_socket;
typedef int wby_socklen;
typedef char wby_sockopt;

#define WBY_INVALID_SOCKET INVALID_SOCKET
#define snprintf _snprintf

#else /* UNIX */

#include <sys/socket.h>
typedef int wby_socket;
typedef socklen_t wby_socklen;
typedef int wby_sockopt;

#define WBY_ALIGN(x) __attribute__((aligned(x)))
#define WBY_INVALID_SOCKET (-1)

#endif

int wby_socket_error();
int wby_socket_is_valid(wby_socket socket);
void wby_socket_close(wby_socket socket);
int wby_socket_is_blocking_error(int error);
int wby_socket_set_blocking(wby_socket socket, int blocking);

int wby_socket_config_incoming(wby_socket socket);
int wby_socket_send(wby_socket socket, const unsigned char *buffer, int size);
int wby_socket_recv(wby_socket socket, struct wby_buffer *buf, wby_log_f log);
int wby_socket_flush(wby_socket socket, struct wby_buffer *buf);

/* Read as much as possible without blocking while there is buffer space. */
enum {
	WBY_FILL_OK,
	WBY_FILL_ERROR,
	WBY_FILL_FULL
};

#endif // MUMUX_SOCKET_H
