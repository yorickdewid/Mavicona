/*
**  Copyright 1998-2003 University of Illinois Board of Trustees
**  Copyright 1998-2003 Mark D. Roth
**  Copyright 2015-2016 Mavicona, Quenza Inc.
**  All rights reserved.
**
**  output.c - print header blocks
**
**  This file is part of the Mavicona project.
**
**  Content can not be copied and/or distributed without the express
**  permission of the author.
*/

#include "libpar.h"

#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <limits.h>
#include <sys/param.h>
#include <string.h>
#include <compat.h>

#ifndef _POSIX_LOGIN_NAME_MAX
# define _POSIX_LOGIN_NAME_MAX	9
#endif

void th_print(PAR *t) {
	puts("\nPrinting header:");
	printf("  name     = \"%.100s\"\n", t->th_buf.name);
	printf("  mode     = \"%.8s\"\n", t->th_buf.mode);
	printf("  uid      = \"%.8s\"\n", t->th_buf.uid);
	printf("  gid      = \"%.8s\"\n", t->th_buf.gid);
	printf("  size     = \"%.12s\"\n", t->th_buf.size);
	printf("  mtime    = \"%.12s\"\n", t->th_buf.mtime);
	printf("  chksum   = \"%.8s\"\n", t->th_buf.chksum);
	printf("  typeflag = \'0x%02x\'\n", t->th_buf.typeflag);
	printf("  linkname = \"%.100s\"\n", t->th_buf.linkname);
	printf("  magic    = \"%.6s\"\n", t->th_buf.magic);
	printf("  version  = \'%c\',\'%c\'\n", t->th_buf.version[0], t->th_buf.version[1]);
	printf("  uname    = \"%.32s\"\n", t->th_buf.uname);
	printf("  gname    = \"%.32s\"\n", t->th_buf.gname);
	printf("  prefix   = \"%.155s\"\n", t->th_buf.prefix);
	printf("  padding  = \"%.12s\"\n", t->th_buf.padding);
	printf("  gnu_longname = \"%s\"\n",
	       (t->th_buf.gnu_longname ? t->th_buf.gnu_longname : "[NULL]"));
	printf("  gnu_longlink = \"%s\"\n",
	       (t->th_buf.gnu_longlink ? t->th_buf.gnu_longlink : "[NULL]"));
}

void th_print_long_ls(PAR *t) {
	char modestring[12];
	struct passwd *pw;
	struct group *gr;
	uid_t uid;
	gid_t gid;
	char username[_POSIX_LOGIN_NAME_MAX];
	char groupname[_POSIX_LOGIN_NAME_MAX];
	time_t mtime;
	struct tm *mtm;
	char timebuf[18];

	uid = th_get_uid(t);
	pw = getpwuid(uid);
	if (pw == NULL)
		snprintf(username, sizeof(username), "%d", uid);
	else
		strlcpy(username, pw->pw_name, sizeof(username));

	gid = th_get_gid(t);
	gr = getgrgid(gid);
	if (gr == NULL)
		snprintf(groupname, sizeof(groupname), "%d", gid);
	else
		strlcpy(groupname, gr->gr_name, sizeof(groupname));

	strmode(th_get_mode(t), modestring);
	printf("%.10s %-8.8s %-8.8s ", modestring, username, groupname);
	printf("%9ld ", (long)th_get_size(t));

	mtime = th_get_mtime(t);
	mtm = localtime(&mtime);

	strftime(timebuf, sizeof(timebuf), "%h %e %H:%M %Y", mtm);
	printf("%s", timebuf);

	printf(" %s", th_get_pathname(t));

	if (TH_ISSYM(t) || TH_ISLNK(t))
	{
		if (TH_ISSYM(t))
			printf(" -> ");
		else
			printf(" link to ");
		if ((t->options & PAR_GNU) && t->th_buf.gnu_longlink != NULL)
			printf("%s", safer_name_suffix(t->th_buf.gnu_longlink));
		else
			printf("%.100s", safer_name_suffix(t->th_buf.linkname));
	}

	putchar('\n');
}
