/**
 * Copyright (C) 2015-2016 Mavicona, Quenza Inc.
 * All Rights Reserved
 *
 * This file is part of the Mavicona project.
 *
 * Content can not be copied and/or distributed without the express
 * permission of the author.
 */

#ifndef MUMUX_SHA_H
#define MUMUX_SHA_H

struct wby_sha1 {
	unsigned int state[5];
	unsigned int msg_size[2];
	unsigned long buf_used;
	unsigned char buffer[64];
};

void wby_sha1_init(struct wby_sha1 *s);
void wby_sha1_update(struct wby_sha1 *s, const void *data_, unsigned long size);
void wby_sha1_final(unsigned char digest[20], struct wby_sha1 *s);

#endif // MUMUX_SHA_H
