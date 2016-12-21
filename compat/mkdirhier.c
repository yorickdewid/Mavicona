/*
**  Copyright 1998-2003 University of Illinois Board of Trustees
**  Copyright 1998-2003 Mark D. Roth
**  Copyright 2015-2016 Mavicona, Quenza Inc.
**  All rights reserved.
**
**  This file is part of the Mavicona project.
**
**  Content can not be copied and/or distributed without the express
**  permission of the author.
**
**  mkdirhier() - create all directories in a given path
**  returns:
**	  0			success
**	  1			all directories already exist
**	  -1 (and sets errno)	error
*/

#include <compat.h>
#include <errno.h>
#include <string.h>

int mkdirhier(char *path) {
	char src[MAXPATHLEN], dst[MAXPATHLEN] = "";
	char *dirp, *nextp = src;
	int retval = 1;

	if (strlcpy(src, path, sizeof(src)) > sizeof(src)) {
		errno = ENAMETOOLONG;
		return -1;
	}

	if (path[0] == '/')
		strcpy(dst, "/");

	while ((dirp = strsep(&nextp, "/")) != NULL) {
		if (*dirp == '\0')
			continue;

		if (dst[0] != '\0')
			strcat(dst, "/");
		strcat(dst, dirp);

		if (mkdir(dst, 0777) == -1) {
			if (errno != EEXIST)
				return -1;
		} else {
			retval = 0;
		}
	}

	return retval;
}
