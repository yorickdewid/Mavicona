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

inline int par_block_read(PAR *t, char *buf) {
	if (t->use_gz)
		return gzread(t->gzfd, buf, T_BLOCKSIZE);

	return read(t->fd, buf, T_BLOCKSIZE);
}

inline int par_block_write(PAR *t, char *buf) {
	if (t->use_gz)
		return gzwrite(t->gzfd, buf, T_BLOCKSIZE);

	return write(t->fd, buf, T_BLOCKSIZE);
}

static gzFile gzopen_init(const char *pathname, int oflags, int mode, int fd) {
	char *gzoflags;
	gzFile gzfd;

	switch (oflags & O_ACCMODE) {
		case O_WRONLY:
			gzoflags = "wb";
			break;
		case O_RDONLY:
			gzoflags = "rb";
			break;
		default:
		case O_RDWR:
			errno = EINVAL;
		return NULL;
	}

	if ((oflags & O_CREAT) && fchmod(fd, mode)) {
		close(fd);
		return NULL;
	}

	gzfd = gzdopen(fd, gzoflags);
	if (!gzfd) {
		errno = ENOMEM;
		return NULL;
	}

	return gzfd;
}

int par_write_header(PAR *t) {
	struct archive_header header;

	strlcpy(header.magic, PACKAGE_MAGIC, sizeof(header.magic));
	header.version_major = PACKAGE_VERSION_MAJOR;
	header.version_minor = PACKAGE_VERSION_MINOR;
	quid_create(&header.quid);

	if (write(t->fd, (char *)&header, sizeof(header)) == -1)
		return -1;

	return 0;
}

int par_read_header(PAR *t) {
	struct archive_header header;

	if (read(t->fd, (char *)&header, sizeof(header)) == -1)
		return -1;

	if (strncmp(header.magic, PACKAGE_MAGIC, sizeof(header.magic)) != 0)
		return -2;

#ifdef DEBUG
	printf("Package version: %d.%d\n", header.version_major, header.version_minor);

	char squid[QUID_LENGTH + 1];
	quidtostr(squid, &header.quid);
	squid[QUID_LENGTH] = '\0';

	printf("Package QUID: %s\n", squid);
#endif

	return 0;
}

static int par_init(PAR **t, const char *pathname, int compress, 
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
	(*t)->oflags = oflags;
	(*t)->use_gz = compress;

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

/* open a new file handle */
int par_open(PAR **t, const char *pathname, int compress,
	 int oflags, int mode, int options) {
	if (par_init(t, pathname, compress, oflags, options) == -1)
		return -1;

	if ((options & PAR_NOOVERWRITE) && (oflags & O_CREAT))
		oflags |= O_EXCL;

	(*t)->fd = open(pathname, oflags, mode);
	if ((*t)->fd == -1)
		return -1;

	if ((*t)->use_gz)
		(*t)->gzfd = gzopen_init(pathname, oflags, mode, (*t)->fd);

	if ((*t)->gzfd == NULL) {
		libtar_hash_free((*t)->h, NULL);
		free(*t);
		return -1;
	}

	return 0;
}

/* close file handle */
int par_close(PAR *t) {
	int i;

	if (t->use_gz)
		i = gzclose(t->gzfd);
	else
		i = close(t->fd);

	if (t->h != NULL)
		libtar_hash_free(t->h, ((t->oflags & O_ACCMODE) == O_RDONLY
					? free : (libtar_freefunc_t)tar_dev_free));
	if (t->th_pathname != NULL)
		free(t->th_pathname);

	free(t);
	return i;
}
