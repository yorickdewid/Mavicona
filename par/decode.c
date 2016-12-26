/*
**  Copyright 1998-2003 University of Illinois Board of Trustees
**  Copyright 1998-2003 Mark D. Roth
**  Copyright 2015-2016 Mavicona, Quenza Inc.
**  All rights reserved.
**
**  decode.c - decode header blocks
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
#include <pwd.h>
#include <grp.h>
#include <string.h>

char *safer_name_suffix(char const *file_name) {
	char const *p, *t;
	p = t = file_name;
	while (*p == '/') t = ++p;
	while (*p) {
		while (p[0] == '.' && p[0] == p[1] && p[2] == '/') {
			p += 3;
			t = p;
		}
		/* advance pointer past the next slash */
		while (*p && (p++)[0] != '/');
	}

	if (!*t) {
		t = ".";
	}

	if (t != file_name) {
		/* TODO: warn somehow that the path was modified */
	}

	return (char*)t;
}

/* determine full path name */
char *th_get_pathname(PAR *t) {
	if (t->th_buf.gnu_longname)
		return safer_name_suffix(t->th_buf.gnu_longname);

	/* allocate the th_pathname buffer if not already */
	if (t->th_pathname == NULL) {
		t->th_pathname = malloc(MAXPATHLEN * sizeof(char));
		if (t->th_pathname == NULL)
			/* out of memory */
			return NULL;
	}

	/*
	 * Old GNU headers (also used by newer GNU tar when doing incremental
	 * dumps) use the POSIX prefix field for many other things, such as
	 * mtime and ctime. New-style GNU headers don't, but also don't use the
	 * POSIX prefix field. Thus, only honor the prefix field if the archive
	 * is actually a POSIX archive. This is the same logic as GNU tar uses.
	 */
	if (strncmp(t->th_buf.magic, TMAGIC, TMAGLEN - 1) != 0 || t->th_buf.prefix[0] == '\0') {
		snprintf(t->th_pathname, MAXPATHLEN, "%.100s", t->th_buf.name);
	} else {
		snprintf(t->th_pathname, MAXPATHLEN, "%.155s/%.100s", t->th_buf.prefix, t->th_buf.name);
	}

	/* will be deallocated in par_close() */
	return safer_name_suffix(t->th_pathname);
}

uid_t th_get_uid(PAR *t) {
	unsigned int uid;
	struct passwd *pw;

	pw = getpwnam(t->th_buf.uname);
	if (pw != NULL)
		return pw->pw_uid;

	/* if the password entry doesn't exist */
	sscanf(t->th_buf.uid, "%o", &uid);
	return uid;
}

gid_t th_get_gid(PAR *t) {
	unsigned int gid;
	struct group *gr;

	gr = getgrnam(t->th_buf.gname);
	if (gr != NULL)
		return gr->gr_gid;

	/* if the group entry doesn't exist */
	sscanf(t->th_buf.gid, "%o", &gid);
	return gid;
}

mode_t th_get_mode(PAR *t) {
	mode_t mode;

	mode = (mode_t)oct_to_int(t->th_buf.mode);
	if (! (mode & S_IFMT)) {
		switch (t->th_buf.typeflag) {
			case SYMTYPE:
				mode |= S_IFLNK;
				break;
			case DIRTYPE:
				mode |= S_IFDIR;
				break;
				/* FALLTHROUGH */
			case LNKTYPE:
			case REGTYPE:
			default:
				mode |= S_IFREG;
		}
	}

	return mode;
}
