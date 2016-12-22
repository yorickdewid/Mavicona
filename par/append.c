/*
**  Copyright 1998-2003 University of Illinois Board of Trustees
**  Copyright 1998-2003 Mark D. Roth
**  Copyright 2015-2016 Mavicona, Quenza Inc.
**  All rights reserved.
**
**  append.c - append files to a archive
**
**  This file is part of the Mavicona project.
**
**  Content can not be copied and/or distributed without the express
**  permission of the author.
*/

#include "libpar.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct tar_dev {
	dev_t td_dev;
	libtar_hash_t *td_h;
};
typedef struct tar_dev tar_dev_t;

struct tar_ino {
	ino_t ti_ino;
	char ti_name[MAXPATHLEN];
};
typedef struct tar_ino tar_ino_t;

/* free memory associated with a tar_dev_t */
void tar_dev_free(tar_dev_t *tdp) {
	libtar_hash_free(tdp->td_h, free);
	free(tdp);
}

/* appends a file to archive */
int par_append_file(PAR *t, const char *realname, const char *savename) {
	struct stat s;
	int i;
	libtar_hashptr_t hp;
	tar_dev_t *td = NULL;
	tar_ino_t *ti = NULL;
	char path[MAXPATHLEN];

#ifdef DEBUG
	printf("==> par_append_file(PAR=0x%p (\"%s\"), realname=\"%s\", "
	       "savename=\"%s\")\n", (void *)t, t->pathname, realname,
	       (savename ? savename : "[NULL]"));
#endif

	if (lstat(realname, &s) != 0) {
#ifdef DEBUG
		perror("lstat()");
#endif
		return -1;
	}

	/* set header block */
#ifdef DEBUG
	puts("    par_append_file(): setting header block...");
#endif
	memset(&(t->th_buf), 0, sizeof(struct par_file_header));
	if (th_set_from_stat(t, &s) != 0) {
#ifdef DEBUG
		puts("    par_append_file(): invalid file type...");
#endif
		return -1;
	}

	/* set the header path */
#ifdef DEBUG
	puts("    par_append_file(): setting header path...");
#endif
	th_set_path(t, (savename ? savename : realname));

	/* check if it's a hardlink */
#ifdef DEBUG
	puts("    par_append_file(): checking inode cache for hardlink...");
#endif
	libtar_hashptr_reset(&hp);
	if (libtar_hash_getkey(t->h, &hp, &(s.st_dev), (libtar_matchfunc_t)dev_match) != 0) {
		td = (tar_dev_t *)libtar_hashptr_data(&hp);
	} else {
#ifdef DEBUG
		printf("+++ adding hash for device (0x%x, 0x%x)...\n",
		       major(s.st_dev), minor(s.st_dev));
#endif
		td = (tar_dev_t *)calloc(1, sizeof(tar_dev_t));
		if (td == NULL)
			return -1;

		td->td_dev = s.st_dev;
		td->td_h = libtar_hash_new(256, (libtar_hashfunc_t)ino_hash);
		if (td->td_h == NULL) {
			free(td);
			return -1;
		}

		if (libtar_hash_add(t->h, td) == -1)
			return -1;
	}

	libtar_hashptr_reset(&hp);
	if (libtar_hash_getkey(td->td_h, &hp, &(s.st_ino), (libtar_matchfunc_t)ino_match) != 0) {
		ti = (tar_ino_t *)libtar_hashptr_data(&hp);
#ifdef DEBUG
		printf("    par_append_file(): encoding hard link \"%s\" "
		       "to \"%s\"...\n", realname, ti->ti_name);
#endif
		t->th_buf.typeflag = LNKTYPE;
		th_set_link(t, ti->ti_name);
	} else {
#ifdef DEBUG
		printf("+++ adding entry: device (0x%x,0x%x), inode %ld "
		       "(\"%s\")...\n", major(s.st_dev), minor(s.st_dev),
		       (long int)s.st_ino, realname);
#endif
		ti = (tar_ino_t *)calloc(1, sizeof(tar_ino_t));
		if (ti == NULL)
			return -1;

		ti->ti_ino = s.st_ino;
		snprintf(ti->ti_name, sizeof(ti->ti_name), "%s", savename ? savename : realname);
		libtar_hash_add(td->td_h, ti);
	}

	/* check if it's a symlink */
	if (TH_ISSYM(t)) {
		i = readlink(realname, path, sizeof(path));
		if (i == -1)
			return -1;
		if (i >= MAXPATHLEN)
			i = MAXPATHLEN - 1;
		path[i] = '\0';
#ifdef DEBUG
		printf("    par_append_file(): encoding symlink \"%s\" -> \"%s\"...\n", realname, path);
#endif
		th_set_link(t, path);
	}

	/* print file info */
	if (t->options & PAR_VERBOSE)
		th_print_long_ls(t);

#ifdef DEBUG
	puts("    par_append_file(): writing header");
#endif
	/* write header */
	if (th_write(t) != 0) {
#ifdef DEBUG
		printf("t->fd = %d\n", t->fd);
#endif
		return -1;
	}
#ifdef DEBUG
	puts("    par_append_file(): back from th_write()");
#endif

	/* if it's a regular file, write the contents as well */
	if (TH_ISREG(t) && par_append_regfile(t, realname) != 0)
		return -1;

	return 0;
}

/* write EOF indicator */
int par_append_eof(PAR *t) {
	int i, j;
	char block[T_BLOCKSIZE];

	memset(&block, 0, T_BLOCKSIZE);
	for (j = 0; j < 2; j++) {
		i = par_block_write(t, (char *)&block);
		if (i != T_BLOCKSIZE) {
			if (i != -1)
				errno = EINVAL;
			return -1;
		}
	}

	return 0;
}

/* add file contents to a archive */
int par_append_regfile(PAR *t, const char *realname) {
	char block[T_BLOCKSIZE];
	int filefd;
	int i, j;
	size_t size;
	int rv = -1;

	filefd = open(realname, O_RDONLY);
	if (filefd == -1) {
#ifdef DEBUG
		perror("open()");
#endif
		return -1;
	}

	size = th_get_size(t);
	for (i = size; i > T_BLOCKSIZE; i -= T_BLOCKSIZE) {
		j = read(filefd, &block, T_BLOCKSIZE);
		if (j != T_BLOCKSIZE) {
			if (j != -1)
				errno = EINVAL;
			goto fail;
		}
		if (par_block_write(t, (char *)&block) == -1)
			goto fail;
	}

	if (i > 0) {
		j = read(filefd, &block, i);
		if (j == -1)
			goto fail;
		memset(&(block[i]), 0, T_BLOCKSIZE - i);
		if (par_block_write(t, (char *)&block) == -1)
			goto fail;
	}

	/* success! */
	rv = 0;
fail:
	close(filefd);

	return rv;
}
