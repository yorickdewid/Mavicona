/**
 * Copyright (C) 2015-2016 Mavicona, Quenza Inc.
 * All Rights Reserved
 *
 * This file is part of the Mavicona project.
 *
 * Content can not be copied and/or distributed without the express
 * permission of the author.
 */

#include "util.h"

int file_exist(const char *name) {
	struct stat buffer;
	return (stat(name, &buffer) == 0);
}

int dir_exist(const char *name) {
	struct stat buffer;
	return (stat(name, &buffer) == 0 && S_ISDIR(buffer.st_mode));
}

char *randstring(size_t length) {
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

int find_in_file(const char *fname, char *str) {
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

const char *file_extension(const char *fname) {
	const char *dot = strrchr(fname, '.');
	if (!dot || dot == fname)
		return NULL;
	return dot + 1;
}

int remove_directory(const char *path) {
	DIR *dir = opendir(path);
	size_t path_len = strlen(path);
	int r = -1;

	if (dir) {
		struct dirent *p;
		r = 0;
		while (!r && (p = readdir(dir))) {
			int r2 = -1;

			/* Skip "." and ".." */
			if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
				continue;

			size_t len = path_len + strlen(p->d_name) + 2; 
			char *buf = (char *)malloc(len);
			if (buf) {
				struct stat statbuf;
				snprintf(buf, len, "%s/%s", path, p->d_name);

				if (!stat(buf, &statbuf)) {
					if (S_ISDIR(statbuf.st_mode))
						r2 = remove_directory(buf);
					else
						r2 = unlink(buf);
				}

				free(buf);
			}

			r = r2;
		}

		closedir(dir);
	}

	if (!r)
		r = rmdir(path);

	return r;
}
