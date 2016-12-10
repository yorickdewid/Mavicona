/*
**  Copyright 1998-2003 University of Illinois Board of Trustees
**  Copyright 1998-2003 Mark D. Roth
**  All rights reserved.
**
**  encode.c - libtar code to encode tar header blocks
**
**  Mark D. Roth <roth@uiuc.edu>
**  Campus Information Technologies and Educational Services
**  University of Illinois at Urbana-Champaign
*/

#include "internal.h"

#include <stdio.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

/* map a file mode to a typeflag */
void th_set_type(PAR *t, mode_t mode) {
	if (S_ISLNK(mode))
		t->th_buf.typeflag = SYMTYPE;
	if (S_ISREG(mode))
		t->th_buf.typeflag = REGTYPE;
	if (S_ISDIR(mode))
		t->th_buf.typeflag = DIRTYPE;
}

/* encode file path */
void th_set_path(PAR *t, const char *pathname) {
	char suffix[2] = "";
	char *tmp;

#ifdef DEBUG
	printf("in th_set_path(th, pathname=\"%s\")\n", pathname);
#endif

	if (t->th_buf.gnu_longname != NULL)
		free(t->th_buf.gnu_longname);
	t->th_buf.gnu_longname = NULL;

	if (pathname[strlen(pathname) - 1] != '/' && TH_ISDIR(t))
		strcpy(suffix, "/");

	if (strlen(pathname) > T_NAMELEN-1 && (t->options & PAR_GNU)) {
		/* GNU-style long name */
		t->th_buf.gnu_longname = strdup(pathname);
		strncpy(t->th_buf.name, t->th_buf.gnu_longname, T_NAMELEN);
	} else if (strlen(pathname) > T_NAMELEN) {
		/* POSIX-style prefix field */
		tmp = strchr(&(pathname[strlen(pathname) - T_NAMELEN - 1]), '/');
		if (tmp == NULL) {
			printf("!!! '/' not found in \"%s\"\n", pathname);
			return;
		}
		snprintf(t->th_buf.name, 100, "%s%s", &(tmp[1]), suffix);
		snprintf(t->th_buf.prefix,
			 ((tmp - pathname + 1) <
			  155 ? (tmp - pathname + 1) : 155), "%s", pathname);
	} else {
		/* classic tar format */
		snprintf(t->th_buf.name, 100, "%s%s", pathname, suffix);
	}

#ifdef DEBUG
	puts("returning from th_set_path()...");
#endif
}


/* encode link path */
void th_set_link(PAR *t, const char *linkname) {
#ifdef DEBUG
	printf("==> th_set_link(th, linkname=\"%s\")\n", linkname);
#endif

	if (strlen(linkname) > T_NAMELEN-1 && (t->options & PAR_GNU)) {
		/* GNU longlink format */
		t->th_buf.gnu_longlink = strdup(linkname);
		strcpy(t->th_buf.linkname, "././@LongLink");
	} else {
		/* classic tar format */
		strlcpy(t->th_buf.linkname, linkname, sizeof(t->th_buf.linkname));
		if (t->th_buf.gnu_longlink != NULL)
			free(t->th_buf.gnu_longlink);
		t->th_buf.gnu_longlink = NULL;
	}
}

/* encode user info */
void th_set_user(PAR *t, uid_t uid) {
	struct passwd *pw;

	pw = getpwuid(uid);
	if (pw != NULL)
		strlcpy(t->th_buf.uname, pw->pw_name, sizeof(t->th_buf.uname));

	int_to_oct(uid, t->th_buf.uid, 8);
}

/* encode group info */
void th_set_group(PAR *t, gid_t gid) {
	struct group *gr;

	gr = getgrgid(gid);
	if (gr != NULL)
		strlcpy(t->th_buf.gname, gr->gr_name, sizeof(t->th_buf.gname));

	int_to_oct(gid, t->th_buf.gid, 8);
}

/* encode file mode */
void th_set_mode(PAR *t, mode_t fmode) {
	int_to_oct(fmode, (t)->th_buf.mode, 8);
}

int th_set_from_stat(PAR *t, struct stat *s) {
	if (S_ISCHR(s->st_mode) || 
		S_ISBLK(s->st_mode) || 
		S_ISFIFO(s->st_mode) || 
		S_ISSOCK(s->st_mode))
		return -1;

	th_set_type(t, s->st_mode);
	th_set_user(t, s->st_uid);
	th_set_group(t, s->st_gid);
	th_set_mode(t, s->st_mode);
	th_set_mtime(t, s->st_mtime);
	
	if (S_ISREG(s->st_mode))
		th_set_size(t, s->st_size);
	else
		th_set_size(t, 0);

	return 0;
}
