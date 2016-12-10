/*
**  Copyright 1998-2003 University of Illinois Board of Trustees
**  Copyright 1998-2003 Mark D. Roth
**  Copyright 2015-2016 Mavicona, Quenza Inc.
**  All rights reserved.
**
**  libtar.c - demo and test driver program for libpar
**
**  This file is part of the Mavicona project.
**
**  Content can not be copied and/or distributed without the express
**  permission of the author.
*/

#include "libpar.h"

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/param.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <compat.h>

int verbose = 0;
int use_gnu = 0;
int use_zlib = 0;

int create(char *tarfile, char *rootdir, libtar_list_t *l) {
	PAR *t;
	char *pathname;
	char buf[MAXPATHLEN];
	libtar_listptr_t lp;

	if (par_open(&t, tarfile, 1, O_WRONLY | O_CREAT, 0644,
		     (verbose ? PAR_VERBOSE : 0)
		     | (use_gnu ? PAR_GNU : 0)) == -1) {
		fprintf(stderr, "tar_open(): %s\n", strerror(errno));
		return -1;
	}

	if (par_write_header(t) == -1) {
		fprintf(stderr, "par_write_header(): %s\n", strerror(errno));
		par_close(t);
		return -1;
	}

	libtar_listptr_reset(&lp);
	while (libtar_list_next(l, &lp) != 0) {
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

int list(char *tarfile) {
	PAR *t;
	int i;

	if (par_open(&t, tarfile, 1, O_RDONLY, 0,
		     (verbose ? PAR_VERBOSE : 0)
		     | (use_gnu ? PAR_GNU : 0)) == -1) {
		fprintf(stderr, "tar_open(): %s\n", strerror(errno));
		return -1;
	}

	if (par_read_header(t) != 0) {
		fprintf(stderr, "par_read_header(): %s\n", strerror(errno));
		par_close(t);
		return -1;
	}

	while ((i = th_read(t)) == 0) {
		th_print_long_ls(t);
#ifdef DEBUG
		th_print(t);
#endif
		if (TH_ISREG(t) && par_skip_regfile(t) != 0) {
			fprintf(stderr, "tar_skip_regfile(): %s\n",
				strerror(errno));
			return -1;
		}
	}

#ifdef DEBUG
	printf("th_read() returned %d\n", i);
	printf("EOF mark encountered after %ld bytes\n",
		(use_zlib ? gzseek((gzFile) t->fd, 0, SEEK_CUR) : lseek(t->fd, 0, SEEK_CUR)));
#endif

	if (par_close(t) != 0) {
		fprintf(stderr, "tar_close(): %s\n", strerror(errno));
		return -1;
	}

	return 0;
}

int extract(char *tarfile, char *rootdir) {
	PAR *t;

	if (par_open(&t, tarfile, 1, O_RDONLY, 0,
		     (verbose ? PAR_VERBOSE : 0)
		     | (use_gnu ? PAR_GNU : 0)) == -1) {
		fprintf(stderr, "tar_open(): %s\n", strerror(errno));
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

void usage(const char *prog) {
	printf("Par " PACKAGE_VERSION " Copyright (C) 2015-2016 Mavicona, Quenza Inc.\n"
		   "All Rights Reserved\n"
		   "usage: %s [OPTION] <file.par> [file...]\n\n"
		   "  -l <file>     Show contents\n"
		   "  -c <file>     Create new archive\n"
		   "  -x <file>     Unpack archive\n"
		   "  -V            Verbose output message\n"
		   "  -v            Show version and exit\n"
		   "  -h            This help message\n", prog);
}

#define MODE_LIST	1
#define MODE_CREATE	2
#define MODE_EXTRACT	3

int main(int argc, char *argv[]) {
	char *tarfile = NULL;
	char *rootdir = NULL;
	int c;
	int mode = 0;
	libtar_list_t *l;
	int return_code = -2;

	while ((c = getopt(argc, argv, "clvVxh")) != -1) {
		switch (c) {
			case 'v':
				puts("libmavpar " PACKAGE_VERSION "\nCopyright 2015-2016 Mavicona, Quenza Inc.\n");
				break;
			// case 'C':
				// rootdir = strdup(optarg);
				// break;
			case 'V':
				verbose = 1;
				break;
			// case 'g':
				// use_gnu = 1;
				// break;
			case 'c':
				if (mode) {
					usage(argv[0]);
					free(rootdir);
					return 1;
				}
				mode = MODE_CREATE;
				break;
			case 'x':
				if (mode){
					usage(argv[0]);
					free(rootdir);
					return 1;
				}
				mode = MODE_EXTRACT;
				break;
			case 'l':
				if (mode) {
					usage(argv[0]);
					free(rootdir);
					return 1;
				}
				mode = MODE_LIST;
				break;

			case 'h':
			default:
				usage(argv[0]);
				free(rootdir);
				return 1;
		}
	}

	if (!mode || ((argc - optind) < (mode == MODE_CREATE ? 2 : 1))) {
		usage(argv[0]);
		free(rootdir);
		return 2;
	}

	switch (mode) {
		case MODE_EXTRACT:
			return_code = extract(argv[optind], rootdir);
			break;
		case MODE_CREATE:
			tarfile = argv[optind];
			l = libtar_list_new(LIST_QUEUE, NULL);
			for (c = optind + 1; c < argc; c++)
				libtar_list_add(l, argv[c]);
			return_code = create(tarfile, rootdir, l);
			libtar_list_free(l, NULL);
			break;
		case MODE_LIST:
			return_code = list(argv[optind]);
			break;
		default:
			break;
	}

	free(rootdir);
	return return_code;
}
