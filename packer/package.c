/**
 * Copyright (C) 2015-2016 Mavicona, Quenza Inc.
 * All Rights Reserved
 *
 * This file is part of the Mavicona project.
 *
 * Content can not be copied and/or distributed without the express
 * permission of the author.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <libpar.h>
#include <compat.h>

int package_create(char *file, char *rootdir, libtar_list_t *list) {
	PAR *t;
	char *pathname;
	char buf[MAXPATHLEN];
	libtar_listptr_t lp;

	if (par_open(&t, file, 1, O_WRONLY | O_CREAT, 0644, 0) == -1) {
		fprintf(stderr, "par_open(): %s\n", strerror(errno));
		return -1;
	}

	if (par_write_header(t) == -1) {
		fprintf(stderr, "par_write_header(): %s\n", strerror(errno));
		par_close(t);
		return -1;
	}

	libtar_listptr_reset(&lp);
	while (libtar_list_next(list, &lp) != 0) {
		pathname = (char *)libtar_listptr_data(&lp);
		if (pathname[0] != '/' && rootdir != NULL)
			snprintf(buf, sizeof(buf), "%s/%s", rootdir, pathname);
		else
			strlcpy(buf, pathname, sizeof(buf));
		
		if (par_append_tree(t, buf, pathname) != 0) {
			fprintf(stderr, "tar_append_tree(\"%s\", \"%s\"): %s\n", buf,
				pathname, strerror(errno));
			par_close(t);
			return -1;
		}
	}

	if (par_append_eof(t) != 0) {
		fprintf(stderr, "tar_append_eof(): %s\n", strerror(errno));
		par_close(t);
		return -1;
	}

	if (par_close(t) != 0) {
		fprintf(stderr, "tar_close(): %s\n", strerror(errno));
		return -1;
	}

	return 0;
}

int package_extract(char *file, char *rootdir) {
	PAR *t;

	if (par_open(&t, file, 1, O_RDONLY, 0, 0) == -1) {
		fprintf(stderr, "par_open(): %s\n", strerror(errno));
		return -1;
	}

	if (par_read_header(t) != 0) {
		fprintf(stderr, "par_read_header(): %s\n", strerror(errno));
		par_close(t);
		return -1;
	}

	if (par_extract_all(t, rootdir) != 0) {
		fprintf(stderr, "par_extract_all(): %s\n", strerror(errno));
		par_close(t);
		return -1;
	}

	if (par_close(t) != 0) {
		fprintf(stderr, "tar_close(): %s\n", strerror(errno));
		return -1;
	}

	return 0;
}

int package_verify(char *file) {
	PAR *t;
	int i;

	if (par_open(&t, file, 1, O_RDONLY, 0, 0) == -1) {
		fprintf(stderr, "par_open(): %s\n", strerror(errno));
		return -1;
	}

	if (par_read_header(t) != 0) {
		fprintf(stderr, "par_read_header(): %s\n", strerror(errno));
		par_close(t);
		return -1;
	}

	while ((i = th_read(t)) == 0) {
		th_print_long_ls(t);
		if (TH_ISREG(t) && par_skip_regfile(t) != 0) {
			fprintf(stderr, "tar_skip_regfile(): %s\n",
				strerror(errno));
			return -1;
		}
	}

	if (par_close(t) != 0) {
		fprintf(stderr, "tar_close(): %s\n", strerror(errno));
		return -1;
	}

	return 0;
}
