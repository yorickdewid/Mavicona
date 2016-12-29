/**
 * Copyright (C) 2015-2016 Mavicona, Quenza Inc.
 * All Rights Reserved
 *
 * This file is part of the Mavicona project.
 *
 * Content can not be copied and/or distributed without the express
 * permission of the author.
 */

#ifndef MUMUX_H
#define MUMUX_H

#ifdef __cplusplus
extern "C" {
#endif

#define WBY_OK (0)
#define WBY_FLAG(x) (1 << (x))

#ifndef WBY_MAX_HEADERS
#define WBY_MAX_HEADERS 64
#endif

struct wby_header {
    const char *name;
    const char *value;
};

/* A HTTP request. */
struct wby_request {
    const char *method;
    /* The method of the request, e.g. "GET", "POST" and so on */
    const char *uri;
    /* The URI that was used. */
    const char *http_version;
    /* The used HTTP version */
    const char *query_params;
    /* The query parameters passed in the URL, or NULL if none were passed. */
    int content_length;
    /* The number of bytes of request body that are available via WebbyRead() */
    int header_count;
    /* The number of headers */
    struct wby_header headers[WBY_MAX_HEADERS];
    /* Request headers */
};

/* Connection state, as published to the serving callback. */
struct wby_con {
    struct wby_request request;
    /* The request being served. Read-only. */
    void *user_data;
    /* User data. Read-write. wby doesn't care about this. */
};

struct wby_frame {
    unsigned char flags;
    unsigned char opcode;
    unsigned char header_size;
    unsigned char padding_;
    unsigned char mask_key[4];
    int payload_length;
};

enum wby_websock_flags {
    WBY_WSF_FIN    = WBY_FLAG(0),
    WBY_WSF_MASKED = WBY_FLAG(1)
};

enum wby_websock_operation {
    WBY_WSOP_CONTINUATION    = 0,
    WBY_WSOP_TEXT_FRAME      = 1,
    WBY_WSOP_BINARY_FRAME    = 2,
    WBY_WSOP_CLOSE           = 8,
    WBY_WSOP_PING            = 9,
    WBY_WSOP_PONG            = 10
};

/* Configuration data required for starting a server. */
typedef void(*wby_log_f)(const char *msg);

struct wby_config {
    void *userdata;
    /* userdata which will be passed */
    const char *address;
    /* The bind address. Must be a textual IP address. */
    unsigned short port;
    /* The port to listen to. */
    unsigned int connection_max;
    /* Maximum number of simultaneous connections. */
    unsigned long request_buffer_size;
    /* The size of the request buffer. This must be big enough to contain all
    * headers and the request line sent by the client. 2-4k is a good size for
    * this buffer. */
    unsigned long io_buffer_size;
    /* The size of the I/O buffer, used when writing the reponse. 4k is a good
    * choice for this buffer.*/
    wby_log_f log;
    /* Optional callback function that receives debug log text (without newlines). */
    int(*dispatch)(struct wby_con *con, void *userdata);
    /* Request dispatcher function. This function is called when the request
    * structure is ready.
    * If you decide to handle the request, call wby_response_begin(),
    * wby_write() and wby_response_end() and then return 0. Otherwise, return a
    * non-zero value to have Webby send back a 404 response. */
    int(*ws_connect)(struct wby_con*, void *userdata);
    /*WebSocket connection dispatcher. Called when an incoming request wants to
    * update to a WebSocket connection.
    * Return 0 to allow the connection.
    * Return 1 to ignore the connection.*/
    void (*ws_connected)(struct wby_con*, void *userdata);
    /* Called when a WebSocket connection has been established.*/
    void (*ws_closed)(struct wby_con*, void *userdata);
    /*Called when a WebSocket connection has been closed.*/
    int (*ws_frame)(struct wby_con*, const struct wby_frame *frame, void *userdata);
    /*Called when a WebSocket data frame is incoming.
    * Call wby_read() to read the payload data.
    * Return non-zero to close the connection.*/
};

struct wby_connection;

struct wby_server {
    struct wby_config config;
    /* server configuration */
    unsigned long memory_size;
    /* minimum required memory */
    unsigned long socket;
    /* server socket */
    unsigned long con_count;
    /* number of active connection */
    struct wby_connection *con;
    /* connections */
};

void wby_init(struct wby_server*, const struct wby_config*,
                            unsigned long *needed_memory);
/*  this function clears the server and calculates the needed memory to run
    Input:
    -   filled server configuration data to calculate the needed memory
    Output:
    -   needed memory for the server to run
*/
int wby_start(struct wby_server*, void *memory);
/*  this function starts running the server in the specificed memory space. Size
 *  must be at least big enough as determined in the wby_server_init().
    Input:
    -   allocated memory space to create the server into
*/
void wby_update(struct wby_server*);
/* updates the server by being called frequenctly (at least once a frame) */
void wby_stop(struct wby_server*);
/* stops and shutdown the server */
int wby_response_begin(struct wby_con*, int status_code, int content_length,
                                    const struct wby_header headers[], int header_count);
/*  this function begins a response
    Input:
    -   HTTP status code to send. (Normally 200).
    -   size in bytes you intend to write, or -1 for chunked encoding
    -   array of HTTP headers to transmit (can be NULL of header_count == 0)
    -   number of headers in the array
    Output:
    -   returns 0 on success, non-zero on error.
*/
void wby_response_end(struct wby_con*);
/*  this function finishes a response. When you're done wirting the response
 *  body, call this function. this makes sure chunked encoding is terminated
 *  correctly and that the connection is setup for reuse. */
int wby_read(struct wby_con*, void *ptr, unsigned long len);
/*  this function reads data from the request body. Only read what the client
 *  has priovided (via content_length) parameter, or you will end up blocking
 *  forever.
    Input:
    - pointer to a memory block that will be filled
    - size of the memory block to fill
*/
int wby_write(struct wby_con*, const void *ptr, unsigned long len);
/*  this function writes a response data to the connection. If you're not using
 *  chunked encoding, be careful not to send more than the specified content
 *  length. You can call this function multiple times as long as the total
 *  number of bytes matches up with the content length.
    Input:
    - pointer to a memory block that will be send
    - size of the memory block to send
*/
int wby_frame_begin(struct wby_con*, int opcode);
/*  this function begins an outgoing websocket frame */
int wby_frame_end(struct wby_con*);
/*  this function finishes an outgoing websocket frame */
int wby_find_query_var(const char *buf, const char *name, char *dst, unsigned long dst_len);
/*  this function is a helper function to lookup a query parameter given a URL
 *  encoded string. Returns the size of the returned data, or -1 if the query
 *  var wasn't found. */
const char *wby_find_header(struct wby_con*, const char *name);
/*  this convenience function to find a header in a request. Returns the value
 *  of the specified header, or NULL if its was not present. */

#ifdef __cplusplus
}
#endif

#endif // MUMUX_H
