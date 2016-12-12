/*
**  Copyright 1998-2003 University of Illinois Board of Trustees
**  Copyright 1998-2003 Mark D. Roth
**  Copyright 2015-2016 Mavicona, Quenza Inc.
**  All rights reserved.
**
**  wrapper.c - libtar high-level wrapper code
**
**  This file is part of the Mavicona project.
**
**  Content can not be copied and/or distributed without the express
**  permission of the author.
*/

#include "libpar.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <dirent.h>
#include <fnmatch.h>
#include <errno.h>
#include <string.h>
#include <compat.h>

int par_extract_glob(PAR *t, char *globname, char *prefix) {
	char *filename;
	char buf[MAXPATHLEN];
	int i;

	while ((i = th_read(t)) == 0) {
		filename = th_get_pathname(t);
		if (fnmatch(globname, filename, FNM_PATHNAME | FNM_PERIOD)) {
			if (TH_ISREG(t) && par_skip_regfile(t))
				return -1;
			continue;
		}

		if (t->options & PAR_VERBOSE)
			th_print_long_ls(t);
		
		if (prefix != NULL)
			snprintf(buf, sizeof(buf), "%s/%s", prefix, filename);
		else
			strlcpy(buf, filename, sizeof(buf));
		
		if (par_extract_file(t, buf) != 0)
			return -1;
	}

	return (i == 1 ? 0 : -1);
}

int par_extract_all(PAR *t, char *prefix) {
	char *filename;
	char buf[MAXPATHLEN];
	int i;

#ifdef DEBUG
	printf("==> tar_extract_all(PAR *t, \"%s\")\n",
	       (prefix ? prefix : "(null)"));
#endif

	while ((i = th_read(t)) == 0)
	{
#ifdef DEBUG
		puts("    tar_extract_all(): calling th_get_pathname()");
#endif
		filename = th_get_pathname(t);
		if (t->options & PAR_VERBOSE)
			th_print_long_ls(t);
		if (prefix != NULL)
			snprintf(buf, sizeof(buf), "%s/%s", prefix, filename);
		else
			strlcpy(buf, filename, sizeof(buf));
#ifdef DEBUG
		printf("    tar_extract_all(): calling tar_extract_file(t, \"%s\")\n", buf);
#endif
		if (par_extract_file(t, buf) != 0)
			return -1;
	}

	return (i == 1 ? 0 : -1);
}

int par_append_tree(PAR *t, char *realdir, char *savedir) {
	int ret = -1;
	char realpath[MAXPATHLEN];
	char savepath[MAXPATHLEN];
	struct dirent *dent;
	DIR *dp;
	struct stat s;

#ifdef DEBUG
	printf("==> par_append_tree(0x%lx, \"%s\", \"%s\")\n",
	       t, realdir, (savedir ? savedir : "[NULL]"));
#endif

	if (par_append_file(t, realdir, savedir) != 0)
		return -1;

#ifdef DEBUG
	puts("    par_append_tree(): done with par_append_file()...");
#endif

	dp = opendir(realdir);
	if (dp == NULL) {
		if (errno == ENOTDIR)
			return 0;
		return -1;
	}

	while ((dent = readdir(dp)) != NULL) {
		if (strcmp(dent->d_name, ".") == 0 ||
		    strcmp(dent->d_name, "..") == 0)
			continue;

		snprintf(realpath, MAXPATHLEN, "%s/%s", realdir, dent->d_name);
		if (savedir)
			snprintf(savepath, MAXPATHLEN, "%s/%s", savedir, dent->d_name);

		if (lstat(realpath, &s) != 0)
			goto out;

		if (S_ISDIR(s.st_mode)) {
			if (par_append_tree(t, realpath, (savedir ? savepath : NULL)) != 0)
				goto out;
			continue;
		}

		if (par_append_file(t, realpath, (savedir ? savepath : NULL)) != 0)
			goto out;
	}

	ret = 0;

out:
	closedir(dp);
	return ret;
}
