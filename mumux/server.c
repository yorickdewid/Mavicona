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
#include "socket.h"
#include "request.h"

#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <memory.h>
#include <errno.h>
#include <strings.h>

/* Pointer to Integer type conversion for pointer alignment */
#if defined(__PTRDIFF_TYPE__) /* This case should work for GCC*/
# define WBY_UINT_TO_PTR(x) ((void*)(__PTRDIFF_TYPE__)(x))
# define WBY_PTR_TO_UINT(x) ((unsigned long)(__PTRDIFF_TYPE__)(x))
#elif !defined(__GNUC__) /* works for compilers other than LLVM */
# define WBY_UINT_TO_PTR(x) ((void*)&((char*)0)[x])
# define WBY_PTR_TO_UINT(x) ((unsigned long)(((char*)x)-(char*)0))
#elif defined(WBY_USE_FIXED_TYPES) /* used if we have <stdint.h> */
# define WBY_UINT_TO_PTR(x) ((void*)(uintptr_t)(x))
# define WBY_PTR_TO_UINT(x) ((uintptr_t)(x))
#else /* generates warning but works */
# define WBY_UINT_TO_PTR(x) ((void*)(x))
# define WBY_PTR_TO_UINT(x) ((unsigned long)(x))
#endif

/* simple pointer math */
#define WBY_PTR_ADD(t, p, i) ((t*)((void*)((unsigned char *)(p) + (i))))
#define WBY_ALIGN_PTR(x, mask)\
    (WBY_UINT_TO_PTR((WBY_PTR_TO_UINT((unsigned char *)(x) + (mask-1)) & ~(mask-1))))

/* pointer alignment  */
#ifdef __cplusplus
template<typename T> struct wby_alignof;
template<typename T, int size_diff> struct wby_helper{enum {value = size_diff};};
template<typename T> struct wby_helper<T,0>{enum {value = wby_alignof<T>::value};};
template<typename T> struct wby_alignof{struct Big {T x; char c;}; enum {
	diff = sizeof(Big) - sizeof(T), value = wby_helper<Big, diff>::value};};
#define WBY_ALIGNOF(t) (wby_alignof<t>::value);
#else
#define WBY_ALIGNOF(t) ((char*)(&((struct {char c; t _h;}*)0)->_h) - (char*)0)
#endif

void wby_init(struct wby_server *srv, const struct wby_config *cfg, unsigned long *needed_memory) {
	WBY_STORAGE const unsigned long wby_conn_align = WBY_ALIGNOF(struct wby_connection);
	WBY_ASSERT(srv);
	WBY_ASSERT(cfg);
	WBY_ASSERT(needed_memory);

	memset(srv, 0, sizeof(*srv));
	srv->config = *cfg;
	WBY_ASSERT(cfg->dispatch);

	*needed_memory = 0;
	*needed_memory += cfg->connection_max * sizeof(struct wby_connection);
	*needed_memory += cfg->connection_max * cfg->request_buffer_size;
	*needed_memory += cfg->connection_max * cfg->io_buffer_size;
	*needed_memory += wby_conn_align;
	srv->memory_size = *needed_memory;
}

int wby_start(struct wby_server *server, void *memory) {
	unsigned long i;
	wby_socket sock;
	wby_sockopt on = 1;
	unsigned char *buffer = (unsigned char *)memory;
	struct sockaddr_in bind_addr;
	WBY_STORAGE const unsigned long wby_conn_align = WBY_ALIGNOF(struct wby_connection);

	WBY_ASSERT(server);
	WBY_ASSERT(memory);
	memset(buffer, 0, server->memory_size);

	/* setup sever memory */
	server->socket = (unsigned long)WBY_INVALID_SOCKET;
	server->con = (struct wby_connection *)WBY_ALIGN_PTR(buffer, wby_conn_align);
	buffer += ((unsigned char *)server->con - buffer);
	buffer += server->config.connection_max * sizeof(struct wby_connection);

	for (i = 0; i < server->config.connection_max; ++i) {
		server->con[i].log = server->config.log;
		server->con[i].header_buf.data = buffer;
		buffer += server->config.request_buffer_size;
		server->con[i].io_buf.data = buffer;
		server->con[i].request_buffer_size = server->config.request_buffer_size;
		server->con[i].io_buffer_size = server->config.io_buffer_size;
		buffer += server->config.io_buffer_size;
	}
	WBY_ASSERT((unsigned long)(buffer - (unsigned char *)memory) <= server->memory_size);

	/* server socket setup */
	sock = (unsigned long)socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	wby_dbg(server->config.log, "Server socket = %d", (int)sock);
	if (!wby_socket_is_valid(sock)) {
		wby_dbg(server->config.log, "failed to initialized server socket: %d", wby_socket_error());
		goto error;
	}

	setsockopt(WBY_SOCK(sock), SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
#ifdef __APPLE__ /* Don't generate SIGPIPE when writing to dead socket, we check all writes. */
	signal(SIGPIPE, SIG_IGN);
#endif
	if (wby_socket_set_blocking(sock, 0) != WBY_OK)
		goto error;

	/* bind server socket */
	wby_dbg(server->config.log, "binding to %s:%d", server->config.address, server->config.port);
	memset(&bind_addr, 0, sizeof(bind_addr));
	bind_addr.sin_family = AF_INET;
	bind_addr.sin_port = htons((unsigned short)server->config.port);
	bind_addr.sin_addr.s_addr = inet_addr(server->config.address);
	if (bind(sock, (struct sockaddr*) &bind_addr, sizeof(bind_addr)) != WBY_OK) {
		wby_dbg(server->config.log, "bind() failed: %d", wby_socket_error());
		wby_dbg(server->config.log, "bind() failed: %s", strerror(wby_socket_error()));
		goto error;
	}

	/* set server socket to listening */
	if (listen(sock, SOMAXCONN) != WBY_OK) {
		wby_dbg(server->config.log, "listen() failed: %d", wby_socket_error());
		wby_socket_close(WBY_SOCK(sock));
		goto error;
	}
	server->socket = (unsigned long)sock;
	wby_dbg(server->config.log, "server initialized: %s", strerror(errno));
	return 0;

error:
	if (wby_socket_is_valid(WBY_SOCK(sock)))
		wby_socket_close(WBY_SOCK(sock));
	return -1;
}

void wby_stop(struct wby_server *srv) {
	unsigned long i;
	wby_socket_close(WBY_SOCK(srv->socket));
	for (i = 0; i < srv->con_count; ++i)
		wby_socket_close(WBY_SOCK(srv->con[i].socket));
}

int wby_on_incoming(struct wby_server *srv) {
	unsigned long connection_index;
	char WBY_ALIGN(8) client_addr[64];
	struct wby_connection* connection;
	wby_socklen client_addr_len = sizeof(client_addr);
	wby_socket fd;

	/* Make sure we have space for a new connection */
	connection_index = srv->con_count;
	if (connection_index == srv->config.connection_max) {
		wby_dbg(srv->config.log, "out of connection slots");
		return 1;
	}

	/* Accept the incoming connection. */
	fd = accept(WBY_SOCK(srv->socket), (struct sockaddr*)&client_addr[0], &client_addr_len);
	if (!wby_socket_is_valid(fd)) {
		int err = wby_socket_error();
		if (!wby_socket_is_blocking_error(err))
			wby_dbg(srv->config.log, "accept() failed: %d", err);
		return 1;
	}

	connection = &srv->con[connection_index];
	wby_connection_reset(connection, srv->config.request_buffer_size, srv->config.io_buffer_size);
	connection->flags = WBY_CON_FLAG_FRESH_CONNECTION;
	srv->con_count = connection_index + 1;

	/* Configure socket */
	if (wby_socket_config_incoming(fd) != WBY_OK) {
		wby_socket_close(fd);
		return 1;
	}

	/* OK, keep this connection */
	wby_dbg(srv->config.log, "tagging connection %d as alive", connection_index);
	connection->flags |= WBY_CON_FLAG_ALIVE;
	connection->socket = (unsigned long)fd;
	return 0;
}

void wby_update_connection(struct wby_server *srv, struct wby_connection* connection) {
	/* This is no longer a fresh connection. Only read from it when select() says
	* so in the future. */
	connection->flags &= (unsigned short)~WBY_CON_FLAG_FRESH_CONNECTION;
	for (;;) {
		switch (connection->state) {
		default: break;
		case WBY_CON_STATE_REQUEST: {
			const char *expect_header;
			int request_size;
			int result = wby_socket_recv(WBY_SOCK(connection->socket),
				&connection->header_buf, srv->config.log);
			if (WBY_FILL_ERROR == result) {
				connection->flags &= (unsigned short)~WBY_CON_FLAG_ALIVE;
				return;
			}

			/* Scan to see if the buffer has a complete HTTP request header package. */
			request_size = wb_peek_request_size(connection->header_buf.data,
				(int)connection->header_buf.used);
			if (request_size < 0) {
				/* Nothing yet. */
				if (connection->header_buf.max == connection->header_buf.used) {
					wby_dbg(srv->config.log, "giving up as buffer is full");
					/* Give up, we can't fit the request in our buffer. */
					connection->flags &= (unsigned short)~WBY_CON_FLAG_ALIVE;
				}
				return;
			}
			wby_dbg(srv->config.log, "peek request size: %d", request_size);


			/* Set up request data. */
			if (wby_connection_setup_request(connection, request_size) != WBY_OK) {
				wby_dbg(srv->config.log, "failed to set up request");
				connection->flags &= (unsigned short)~WBY_CON_FLAG_ALIVE;
				return;
			}

			/* Remember how much of the remaining buffer is body data. */
			connection->header_body_left = (int)connection->header_buf.used - request_size;
			/* If the client expects a 100 Continue, send one now. */
			if (NULL != (expect_header = wby_find_header(&connection->public_data, "Expect"))) {
				if (!strcasecmp(expect_header, "100-continue")) {
					wby_dbg(srv->config.log, "connection expects a 100 Continue header.. making him happy");
					connection->continue_data_left = (int)wby_continue_header_len;
					connection->state = WBY_CON_STATE_SEND_CONTINUE;
				} else {
					wby_dbg(srv->config.log, "unrecognized Expected header %s", expect_header);
					connection->state = WBY_CON_STATE_SERVE;
				}
			} else connection->state = WBY_CON_STATE_SERVE;
		} break; /* WBY_REQUEST */

		case WBY_CON_STATE_SEND_CONTINUE: {
			int left = connection->continue_data_left;
			int offset = (int)wby_continue_header_len - left;
			long written = 0;

			written = send(WBY_SOCK(connection->socket), wby_continue_header + offset, (unsigned long)left, 0);
			wby_dbg(srv->config.log, "continue write: %d bytes", written);
			if (written < 0) {
				wby_dbg(srv->config.log, "failed to write 100-continue header");
				connection->flags &= (unsigned short)~WBY_CON_FLAG_ALIVE;
				return;
			}
			left -= (int)written;
			connection->continue_data_left = left;
			if (left == 0)
				connection->state = WBY_CON_STATE_SERVE;
		} break; /* WBY_SEND_cONTINUE */

		case WBY_CON_STATE_SERVE: {
			/* Clear I/O buffer for output */
			connection->io_buf.used = 0;
			/* Switch socket to blocking mode. */
			if (wby_connection_set_blocking(connection) != WBY_OK)
				return;

			/* Figure out if this is a request to upgrade to WebSockets */
			if (wby_con_is_websocket_request(&connection->public_data)) {
				wby_dbg(srv->config.log, "received a websocket upgrade request");
				if (!srv->config.ws_connect ||
					srv->config.ws_connect(&connection->public_data, srv->config.userdata) != WBY_OK)
				{
					wby_dbg(srv->config.log, "user callback failed connection attempt");
					wby_response_begin(&connection->public_data, 400, -1,
						wby_plain_text_headers, WBY_LEN(wby_plain_text_headers));
					wby_printf(&connection->public_data, "WebSockets not supported at %s\r\n",
						connection->public_data.request.uri);
					wby_response_end(&connection->public_data);
				} else {
					/* OK, let's try to upgrade the connection to WebSockets */
					if (wby_connection_send_websocket_upgrade(connection) != WBY_OK) {
						wby_dbg(srv->config.log, "websocket upgrade failed");
						wby_response_begin(&connection->public_data, 400, -1,
							wby_plain_text_headers, WBY_LEN(wby_plain_text_headers));
						wby_printf(&connection->public_data, "WebSockets couldn't not be enabled\r\n");
						wby_response_end(&connection->public_data);
					} else {
						/* OK, we're now a websocket */
						connection->flags |= WBY_CON_FLAG_WEBSOCKET;
						wby_dbg(srv->config.log, "connection %d upgraded to websocket",
							(int)(connection - srv->con));
						srv->config.ws_connected(&connection->public_data, srv->config.userdata);
					}
				}
			} else if (srv->config.dispatch(&connection->public_data, srv->config.userdata) != 0) {
				static const struct wby_header headers[] = {{ "Content-Type", "text/plain" }};
				wby_response_begin(&connection->public_data, 404, -1, headers, WBY_LEN(headers));
				wby_printf(&connection->public_data, "No handler for %s\r\n",
					connection->public_data.request.uri);
				wby_response_end(&connection->public_data);
			}

			/* Back to non-blocking mode, can make the socket die. */
			wby_connection_set_nonblocking(connection);
			/* Ready for another request, unless we should close the connection. */
			if (connection->flags & WBY_CON_FLAG_ALIVE) {
				if (connection->flags & WBY_CON_FLAG_CLOSE_AFTER_RESPONSE) {
					connection->flags &= (unsigned short)~WBY_CON_FLAG_ALIVE;
					return;
				} else {
					/* Reset connection for next request. */
					wby_connection_reset(connection, srv->config.request_buffer_size,
						srv->config.io_buffer_size);
					if (!(connection->flags & WBY_CON_FLAG_WEBSOCKET)) {
						/* Loop back to request state */
						connection->state = WBY_CON_STATE_REQUEST;
					} else {
						/* Clear I/O buffer for input */
						connection->io_buf.used = 0;
						/* Go to the web socket serving state */
						connection->state = WBY_CON_STATE_WEBSOCKET;
					}
				}
			}
		} break; /* WBY_SERVE */

		case WBY_CON_STATE_WEBSOCKET: {
			/* In this state, we're trying to read a websocket frame into the I/O
			* buffer. Once we have enough data, we call the websocket frame
			* callback and let the client read the data through WebbyRead. */
			if (WBY_FILL_ERROR == wby_socket_recv(WBY_SOCK(connection->socket),
				&connection->io_buf, srv->config.log)) {
				/* Give up on this connection */
				connection->flags &= (unsigned short)~WBY_CON_FLAG_ALIVE;
				return;
			}

			if (wby_scan_websocket_frame(&connection->ws_frame, &connection->io_buf) != WBY_OK)
				return;

			connection->body_bytes_read = 0;
			connection->io_data_left = (int)connection->io_buf.used - connection->ws_frame.header_size;
			wby_dbg(srv->config.log, "%d bytes of incoming websocket data buffered",
				(int)connection->io_data_left);

			/* Switch socket to blocking mode */
			if (wby_connection_set_blocking(connection) != WBY_OK)
				return;

			switch (connection->ws_frame.opcode)
			{
			case WBY_WSOP_CLOSE:
				wby_dbg(srv->config.log, "received websocket close request");
				connection->flags &= (unsigned short)~WBY_CON_FLAG_ALIVE;
				return;

			  case WBY_WSOP_PING:
				wby_dbg(srv->config.log, "received websocket ping request");
				if (wby_socket_send(WBY_SOCK(connection->socket), wby_websocket_pong,
					sizeof wby_websocket_pong)){
					connection->flags &= (unsigned short)~WBY_CON_FLAG_ALIVE;
					return;
				}
				break;

			  default:
				/* Dispatch frame to user handler. */
				if (srv->config.ws_frame(&connection->public_data,
					&connection->ws_frame, srv->config.userdata) != WBY_OK) {
				  connection->flags &= (unsigned short)~WBY_CON_FLAG_ALIVE;
				  return;
				}
			}

			/* Discard any data the client didn't read to retain the socket state. */
			if (connection->body_bytes_read < connection->ws_frame.payload_length) {
				int size = connection->ws_frame.payload_length - connection->body_bytes_read;
				if (wby_con_discard_incoming_data(&connection->public_data, size) != WBY_OK) {
					connection->flags &= (unsigned short)~WBY_CON_FLAG_ALIVE;
					return;
				}
			}

			/* Back to non-blocking mode */
			if (wby_connection_set_nonblocking(connection) != WBY_OK)
				return;

			wby_connection_reset(connection, srv->config.request_buffer_size, srv->config.io_buffer_size);
			connection->state = WBY_CON_STATE_WEBSOCKET;
		} break; /* WBY_WEBSOCKET */
		} /* switch */
	} /* for */
}

void wby_update(struct wby_server *srv) {
	int err;
	unsigned long i, count;
	wby_socket max_socket;
	fd_set read_fds, write_fds, except_fds;
	struct timeval timeout;

	/* Build set of sockets to check for events */
	FD_ZERO(&read_fds);
	FD_ZERO(&write_fds);
	FD_ZERO(&except_fds);
	max_socket = 0;

	/* Only accept incoming connections if we have space */
	if (srv->con_count < srv->config.connection_max) {
		FD_SET(srv->socket, &read_fds);
		FD_SET(srv->socket, &except_fds);
		max_socket = WBY_SOCK(srv->socket);
	}

	for (i = 0, count = srv->con_count; i < count; ++i) {
		wby_socket socket = WBY_SOCK(srv->con[i].socket);
		FD_SET(socket, &read_fds);
		FD_SET(socket, &except_fds);
		if (srv->con[i].state == WBY_CON_STATE_SEND_CONTINUE)
			FD_SET(socket, &write_fds);
		if (socket > max_socket)
			max_socket = socket;
	}

	timeout.tv_sec = 0;
	timeout.tv_usec = 5;
	err = select((int)(max_socket + 1), &read_fds, &write_fds, &except_fds, &timeout);
	if (err < 0) {
		wby_dbg(srv->config.log, "failed to select");
		return;
	}

	/* Handle incoming connections */
	if (FD_ISSET(WBY_SOCK(srv->socket), &read_fds)) {
		do {
			wby_dbg(srv->config.log, "awake on incoming");
			err = wby_on_incoming(srv);
		} while (err == 0);
	}

	/* Handle incoming connection data */
	for (i = 0, count = srv->con_count; i < count; ++i) {
		struct wby_connection *conn = &srv->con[i];
		if (FD_ISSET(WBY_SOCK(conn->socket), &read_fds) ||
			FD_ISSET(WBY_SOCK(conn->socket), &write_fds) ||
			conn->flags & WBY_CON_FLAG_FRESH_CONNECTION)
		{
			wby_dbg(srv->config.log, "reading from connection %d", i);
			wby_update_connection(srv, conn);
		}
	}

	/* Close stale connections & compact connection array. */
	for (i = 0; i < srv->con_count; ) {
		struct wby_connection *connection = &srv->con[i];
		if (!(connection->flags & WBY_CON_FLAG_ALIVE)) {
			unsigned long remain;
			wby_dbg(srv->config.log, "closing connection %d (%08x)", i, connection->flags);
			if (connection->flags & WBY_CON_FLAG_WEBSOCKET)
				srv->config.ws_closed(&connection->public_data, srv->config.userdata);
			remain = srv->con_count - (unsigned long)i - 1;
			wby_connection_close(connection);
			memmove(&srv->con[i], &srv->con[i + 1], remain*sizeof(srv->con[i]));
			--srv->con_count;
		} else ++i;
	}
}
