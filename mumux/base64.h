/**
 * Copyright (C) 2015-2016 Mavicona, Quenza Inc.
 * All Rights Reserved
 *
 * This file is part of the Mavicona project.
 *
 * Content can not be copied and/or distributed without the express
 * permission of the author.
 */

#ifndef MUMUX_BASE64_H
#define MUMUX_BASE64_H

int wby_base64_encode(char *output, unsigned long output_size,
	const unsigned char *input, unsigned long input_size);

#endif // MUMUX_BASE64_H
