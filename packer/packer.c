#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <libtar.h>

#define VERSION "1.0"

int use_gnu = 0;
int verbose = 0;

int find_in_file(char *fname, char *str) {
	FILE *fp;
	int line_num = 1;
	int find_result = 0;
	char temp[512];

	if ((fp = fopen(fname, "r")) == NULL) {
		return (-1);
	}

	while (fgets(temp, 512, fp) != NULL) {
		if ((strstr(temp, str)) != NULL) {
			find_result++;
		}
		line_num++;
	}

	if (fp) {
		fclose(fp);
	}
	return find_result;
}

int create(char *tarfile, char *rootdir, libtar_list_t *l) {
	TAR *t;
	char *pathname;
	char buf[1024];
	libtar_listptr_t lp;

	if (tar_open(&t, tarfile, NULL, O_WRONLY | O_CREAT, 0644, (verbose ? TAR_VERBOSE : 0) | (use_gnu ? TAR_GNU : 0)) == -1) {
		fprintf(stderr, "tar_open(): %s\n", strerror(errno));
		return -1;
	}

	libtar_listptr_reset(&lp);
	while (libtar_list_next(l, &lp) != 0) {
		pathname = (char *)libtar_listptr_data(&lp);
		if (pathname[0] != '/' && rootdir != NULL)
			snprintf(buf, sizeof(buf), "%s/%s", rootdir, pathname);
		else
			strlcpy(buf, pathname, sizeof(buf));
		if (tar_append_tree(t, buf, pathname) != 0) {
			fprintf(stderr,
			        "tar_append_tree(\"%s\", \"%s\"): %s\n", buf,
			        pathname, strerror(errno));
			tar_close(t);
			return -1;
		}
	}

	if (tar_append_eof(t) != 0) {
		fprintf(stderr, "tar_append_eof(): %s\n", strerror(errno));
		tar_close(t);
		return -1;
	}

	if (tar_close(t) != 0) {
		fprintf(stderr, "tar_close(): %s\n", strerror(errno));
		return -1;
	}

	return 0;
}

void usage(const char *prog) {
	printf("JobPacker " VERSION " Copyright (C) 2015-2016 Mavicona, Quenza Inc.\n"
	       "All Rights Reserved\n"
	       "usage: %s <job> [source...]\n", prog);
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		usage(argv[0]);
		return 1;
	}

	int c, f = 0;
	libtar_list_t *l = libtar_list_new(LIST_QUEUE, NULL);
	for (c = 2; c < argc; ++c) {
		libtar_list_add(l, argv[c]);

		if (find_in_file(argv[c], "job_main()") > 0) {
			f = 1;
		}
	}

	if (!f) {
		libtar_list_free(l, NULL);
		fprintf(stderr, "missing job_main()\n");
		return 1;
	}

	int return_code = create(argv[1], ".", l);
	libtar_list_free(l, NULL);

	// return extract(argv[1], ".");
	// return create(argv[1], ".");
	return return_code;
}
