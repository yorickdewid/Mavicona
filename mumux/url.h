/**
 * Copyright (C) 2015-2016 Mavicona, Quenza Inc.
 * All Rights Reserved
 *
 * This file is part of the Mavicona project.
 *
 * Content can not be copied and/or distributed without the express
 * permission of the author.
 */

#ifndef MUMUX_URL_H
#define MUMUX_URL_H

unsigned long wby_url_decode(const char *src, unsigned long src_len, char *dst, unsigned long dst_len,
    int is_form_url_encoded);

int wby_find_query_var(const char *buf, const char *name, char *dst, unsigned long dst_len);

#endif // MUMUX_URL_H
