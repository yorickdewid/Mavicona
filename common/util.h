/**
 * Copyright (C) 2015-2016 Mavicona, Quenza Inc.
 * All Rights Reserved
 *
 * This file is part of the Mavicona project.
 *
 * Content can not be copied and/or distributed without the express
 * permission of the author.
 */

#ifndef UTIL_H
#define UTIL_H

#ifdef __cplusplus
#include <algorithm>
#include <string>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#ifdef __cplusplus
extern "C" {
#endif

int file_exist(const char *name);
int dir_exist(const char *name);

char *randstring(size_t length);
int find_in_file(const char *fname, char *str);
const char *file_extension(const char *fname);
int remove_directory(const char *path);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
inline std::string normalize(std::string& str) {
	std::transform(str.begin(), str.end(), str.begin(), [](char c) {
		return tolower(c);
	});

	return str;
}
#endif

#endif // UTIL_H
