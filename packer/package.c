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
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <libtar.h>
#include <zlib.h>
#include <time.h>

#define PKGVER 0x3

int use_gnu = 0;
int use_zlib = 1;
int verbose = 0;

struct jobheader {
	uint8_t signature[8];
	uint8_t version;
	uint8_t compression;
};

const unsigned char magic[8] = {0x17, 'M', 'A', 'V', 'J', 'O', 'B', 0x80};

static gzFile gzopen_frontend(char *pathname, int oflags, int mode) {
	char *gzoflags;
	gzFile gzf;
	int fd;

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

	fd = open(pathname, oflags, mode);
	if (fd == -1)
		return NULL;

	if ((oflags & O_CREAT) && fchmod(fd, mode)) {
		close(fd);
		return NULL;
	}

	gzf = gzdopen(fd, gzoflags);
	if (!gzf) {
		errno = ENOMEM;
		return NULL;
	}

	/* This is a bad thing to do on big-endian lp64 systems, where the
	   size and placement of integers is different than pointers.
	   However, to fix the problem 4 wrapper functions would be needed and
	   an extra bit of data associating GZF with the wrapper functions.  */
	return gzf;
}

tartype_t gztype = {
	(openfunc_t)gzopen_frontend,
	(closefunc_t)gzclose,
	(readfunc_t)gzread,
	(writefunc_t)gzwrite
};

static char *randstring(size_t length) {
	static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-";
	char *randstr = NULL;

	srand(time(NULL));

	if (!length)
		length = 8;

	randstr = malloc(sizeof(char) * (length + 1));
	if (!randstr)
		return NULL;

	unsigned int n;
	for (n = 0; n < length; ++n) {
		int key = rand() % (int)(sizeof(charset) - 1);
		randstr[n] = charset[key];
	}

	randstr[length] = '\0';

	return randstr;
}

static int verify_header(struct jobheader *header) {
	if (memcmp(header->signature, magic, 8)) {
		fprintf(stderr, "not a mavicona package\n");
		return 1;
	}

	if (header->version != PKGVER) {
		fprintf(stderr, "invalid package version\n");
		return 1;
	}

	return 0;
}

static void append_header(const char *tarfile) {
	FILE *fp = fopen(tarfile, "r+b");
	if (!fp)
		return;

	char jobfile[1024];
	snprintf(jobfile, sizeof(jobfile), "%s.job", tarfile);
	FILE *fpo = fopen(jobfile, "wb");
	if (!fpo)
		return;

	fseek(fp, 0, SEEK_END);
	size_t fsize = ftell(fp);
	rewind(fp);

	uint8_t *buffer = (uint8_t *)malloc(sizeof(uint8_t) * fsize);
	if (!buffer)
		return;

	fread(buffer, 1, fsize, fp);

	struct jobheader header;
	strncpy((char *)header.signature, (const char *)magic, 8);
	header.version = PKGVER;
	header.compression = 0;

	fwrite(&header, 1, sizeof(struct jobheader), fpo);
	fwrite(buffer, 1, fsize, fpo);

	free(buffer);
	fclose(fpo);
	fclose(fp);
}

static void remove_header(const char *jobfile, const char *tmpfile) {
	FILE *fp = fopen(jobfile, "rb");
	if (!fp)
		return;

	struct jobheader header;
	fread(&header, 1, sizeof(struct jobheader), fp);
	if (verify_header(&header)) {
		fclose(fp);
		return;
	}

	rewind(fp);

	FILE *fpo = fopen(tmpfile, "wb");
	if (!fpo)
		return;

	fseek(fp, 0, SEEK_END);
	size_t fsize = ftell(fp);
	fseek(fp, sizeof(struct jobheader), SEEK_SET);

	uint8_t *buffer = (uint8_t *)malloc(sizeof(uint8_t) * (fsize - sizeof(struct jobheader)));
	if (!buffer)
		return;

	fread(buffer, 1, (fsize - sizeof(struct jobheader)), fp);
	fwrite(buffer, 1, fsize, fpo);

	free(buffer);
	fclose(fpo);
	fclose(fp);
}

int package_create(const char *tarfile, char *rootdir, libtar_list_t *list) {
	TAR *tar;
	char buf[1024];
	libtar_listptr_t listpointer;

	if (tar_open(&tar, tarfile, (use_zlib ? &gztype : NULL), O_WRONLY | O_CREAT, 0644, (verbose ? TAR_VERBOSE : 0) | (use_gnu ? TAR_GNU : 0)) == -1) {
		fprintf(stderr, "tar_open(): %s\n", strerror(errno));
		return 1;
	}

	libtar_listptr_reset(&listpointer);
	while (libtar_list_next(list, &listpointer) != 0) {
		char *pathname = (char *)libtar_listptr_data(&listpointer);
		if (pathname[0] != '/' && rootdir != NULL)
			snprintf(buf, sizeof(buf), "%s/%s", rootdir, pathname);
		else
			strncpy(buf, pathname, sizeof(buf));
		
		if (tar_append_tree(tar, buf, pathname) != 0) {
			fprintf(stderr, "tar_append_tree(\"%s\", \"%s\"): %s\n", buf, pathname, strerror(errno));
			tar_close(tar);
			return 1;
		}
	}

	if (tar_append_eof(tar) != 0) {
		fprintf(stderr, "tar_append_eof(): %s\n", strerror(errno));
		tar_close(tar);
		return 1;
	}

	if (tar_close(tar) != 0) {
		fprintf(stderr, "tar_close(): %s\n", strerror(errno));
		return 1;
	}

	append_header(tarfile);

	unlink(tarfile);

	return 0;
}

int package_extract(const char *jobfile, char *rootdir) {
	TAR *tar;
	char dirname[1024];
	char *tarfile = randstring(12);

	remove_header(jobfile, tarfile);

	if (!rootdir) {
		strcpy(dirname, "package_");
		strcat(dirname, tarfile);
	} else {
		strcpy(dirname, rootdir);
	}

	mkdir(dirname, 0775);

	if (tar_open(&tar, tarfile, (use_zlib ? &gztype : NULL), O_RDONLY, 0, (verbose ? TAR_VERBOSE : 0) | (use_gnu ? TAR_GNU : 0)) == -1) {
		fprintf(stderr, "tar_open(): %s\n", strerror(errno));
		return 1;
	}

	if (tar_extract_all(tar, dirname) != 0) {
		fprintf(stderr, "tar_extract_all(): %s\n", strerror(errno));
		tar_close(tar);
		return 1;
	}

	if (tar_close(tar) != 0) {
		fprintf(stderr, "tar_close(): %s\n", strerror(errno));
		return 1;
	}

	unlink(tarfile);

	return 0;
}

int package_verify(const char *pkgfile) {
	FILE *fp = fopen(pkgfile, "rb");
	if (!fp)
		return 1;

	struct jobheader header;
	fread(&header, 1, sizeof(struct jobheader), fp);
	if (verify_header(&header)) {
		fclose(fp);
		return 1;
	}

	fclose(fp);

	puts("Package is OK");

	return 0;
}

void package_info(const char *pkgfile) {
	FILE *fp = fopen(pkgfile, "rb");
	if (!fp)
		return;

	struct jobheader header;
	fread(&header, 1, sizeof(struct jobheader), fp);
	if (verify_header(&header)) {
		fclose(fp);
		return;
	}

	printf(
		"Version: %u\n"
		"Use compression: %s\n"
		, header.version
		, header.compression  ? "Yes" : "No");

	fclose(fp);
}

void package_set_verbose(int f) {
	verbose = f;
}

void package_set_compression(int f) {
	use_zlib = f;
}
