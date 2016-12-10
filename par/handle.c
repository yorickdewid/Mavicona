/*
**  Copyright 1998-2003 University of Illinois Board of Trustees
**  Copyright 1998-2003 Mark D. Roth
**  Copyright 2015-2016 Mavicona, Quenza Inc.
**  All rights reserved.
**
**  handle.c - initializing or closing a PAR handle
**
**  This file is part of the Mavicona project.
**
**  Content can not be copied and/or distributed without the express
**  permission of the author.
*/

#include "internal.h"

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory.h>

static partype_t default_type = {open, close, read, write};

int par_write_header(PAR *t) {
	struct archive_header header;

	strlcpy(header.magic, PACKAGE_MAGIC, sizeof(header.magic));
	header.version_major = PACKAGE_VERSION_MAJOR;
	header.version_minor = PACKAGE_VERSION_MINOR;
	memset(header.quid, '\0', sizeof(header.quid));

	write(t->fd, (const void *)(&header), sizeof(struct archive_header));
	return 0;
}

static int par_init(PAR **t, const char *pathname, partype_t *type, 
	int oflags, int options) {
	if ((oflags & O_ACCMODE) == O_RDWR) {
		errno = EINVAL;
		return -1;
	}

	*t = (PAR *)calloc(1, sizeof(PAR));
	if (*t == NULL)
		return -1;

	(*t)->pathname = pathname;
	(*t)->options = options;
	(*t)->type = (type ? type : &default_type);
	(*t)->oflags = oflags;

	if ((oflags & O_ACCMODE) == O_RDONLY)
		(*t)->h = libtar_hash_new(256, (libtar_hashfunc_t)path_hashfunc);
	else
		(*t)->h = libtar_hash_new(16, (libtar_hashfunc_t)dev_hash);
	
	if ((*t)->h == NULL) {
		free(*t);
		return -1;
	}

	return 0;
}

/* open a new tarfile handle */
int par_open(PAR **t, const char *pathname, partype_t *type,
	 int oflags, int mode, int options) {
	if (par_init(t, pathname, type, oflags, options) == -1)
		return -1;

	if ((options & PAR_NOOVERWRITE) && (oflags & O_CREAT))
		oflags |= O_EXCL;

	(*t)->fd = (*((*t)->type->openfunc))(pathname, oflags, mode);
	if ((*t)->fd == -1) {
		libtar_hash_free((*t)->h, NULL);
		free(*t);
		return -1;
	}

	return 0;
}

/* close tarfile handle */
int par_close(PAR *t) {
	int i;

	i = (*(t->type->closefunc))(t->fd);

	if (t->h != NULL)
		libtar_hash_free(t->h, ((t->oflags & O_ACCMODE) == O_RDONLY
					? free : (libtar_freefunc_t)tar_dev_free));
	if (t->th_pathname != NULL)
		free(t->th_pathname);

	free(t);
	return i;
}
