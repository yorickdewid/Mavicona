/*
**  Copyright 1998-2003 University of Illinois Board of Trustees
**  Copyright 1998-2003 Mark D. Roth
**  Copyright 2015-2016 Mavicona, Quenza Inc.
**  All rights reserved.
**
**  extract.c - extract a file from archive
**
**  This file is part of the Mavicona project.
**
**  Content can not be copied and/or distributed without the express
**  permission of the author.
*/

#include "libpar.h"
#define OS_UNKNOWN 1
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <utime.h>
#include <stdlib.h>
#include <unistd.h>
#include <compat.h>

static int par_set_file_perms(PAR *t, char *realname) {
	mode_t mode;
	uid_t uid;
	gid_t gid;
	struct utimbuf ut;
	char *filename;

	filename = (realname ? realname : th_get_pathname(t));
	mode = th_get_mode(t);
	uid = th_get_uid(t);
	gid = th_get_gid(t);
	ut.modtime = ut.actime = th_get_mtime(t);

	/* change owner/group */
	if (geteuid() == 0)
		if (!TH_ISSYM(t) && chown(filename, uid, gid) == -1) {
# ifdef DEBUG
			fprintf(stderr, "chown(\"%s\", %d, %d): %s\n",
				filename, uid, gid, strerror(errno));
# endif
			return -1;
		}

	/* change access/modification time */
	if (!TH_ISSYM(t) && utime(filename, &ut) == -1) {
#ifdef DEBUG
		perror("utime()");
#endif
		return -1;
	}

	/* change permissions */
	if (!TH_ISSYM(t) && chmod(filename, mode) == -1) {
#ifdef DEBUG
		perror("chmod()");
#endif
		return -1;
	}

	return 0;
}

/* switchboard */
int par_extract_file(PAR *t, char *realname) {
	int i;
	char *lnp;
	int pathname_len;
	int realname_len;

	if (t->options & PAR_NOOVERWRITE) {
		struct stat s;

		if (lstat(realname, &s) == 0 || errno != ENOENT) {
			errno = EEXIST;
			return -1;
		}
	}

	if (TH_ISDIR(t)) {
		i = par_extract_dir(t, realname);
		if (i == 1)
			i = 0;
	} else if (TH_ISLNK(t)) {
		i = par_extract_hardlink(t, realname);
	} else if (TH_ISSYM(t)) {
		i = par_extract_symlink(t, realname);
	} else { /* if (TH_ISREG(t)) */
		i = par_extract_regfile(t, realname);
	}

	if (i != 0)
		return i;

	i = par_set_file_perms(t, realname);
	if (i != 0)
		return i;

	pathname_len = strlen(th_get_pathname(t)) + 1;
	realname_len = strlen(realname) + 1;
	lnp = (char *)calloc(1, pathname_len + realname_len);
	if (!lnp)
		return -1;

	strcpy(&lnp[0], th_get_pathname(t));
	strcpy(&lnp[pathname_len], realname);
#ifdef DEBUG
	printf("par_extract_file(): calling libtar_hash_add(): key=\"%s\", "
	       "value=\"%s\"\n", th_get_pathname(t), realname);
#endif
	if (libtar_hash_add(t->h, lnp) != 0)
		return -1;

	return 0;
}

/* extract regular file */
int par_extract_regfile(PAR *t, char *realname) {
	size_t size;
#ifdef DEBUG
	mode_t mode;
	uid_t uid;
	gid_t gid;
#endif
	int fdout;
	ssize_t i, k;
	char buf[T_BLOCKSIZE];
	char *filename;

#ifdef DEBUG
	printf("==> par_extract_regfile(t=0x%p, realname=\"%s\")\n", (void *)t,
	       realname);
#endif

	if (!TH_ISREG(t)) {
		errno = EINVAL;
		return -1;
	}

	filename = (realname ? realname : th_get_pathname(t));
	size = th_get_size(t);
#ifdef DEBUG
	mode = th_get_mode(t);
	uid = th_get_uid(t);
	gid = th_get_gid(t);
#endif

	if (mkdirhier(cdirname(filename)) == -1)
		return -1;

#ifdef DEBUG
	printf("  ==> extracting: %s (mode %04o, uid %d, gid %d, %zu bytes)\n",
	       filename, mode, uid, gid, size);
#endif
	fdout = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (fdout == -1) {
#ifdef DEBUG
		perror("open()");
#endif
		return -1;
	}

	/* extract the file */
	for (i = size; i > 0; i -= T_BLOCKSIZE) {
		k = par_block_read(t, buf);
		if (k != T_BLOCKSIZE) {
			if (k != -1)
				errno = EINVAL;
			close(fdout);
			return -1;
		}

		/* write block to output file */
		if (write(fdout, buf, ((i > T_BLOCKSIZE) ? T_BLOCKSIZE : i)) == -1) {
			close(fdout);
			return -1;
		}
	}

	/* close output file */
	if (close(fdout) == -1)
		return -1;

#ifdef DEBUG
	printf("### done extracting %s\n", filename);
#endif

	return 0;
}

/* skip regfile */
int par_skip_regfile(PAR *t) {
	ssize_t i, k;
	size_t size;
	char buf[T_BLOCKSIZE];

	if (!TH_ISREG(t)) {
		errno = EINVAL;
		return -1;
	}

	size = th_get_size(t);
	for (i = size; i > 0; i -= T_BLOCKSIZE) {
		k = par_block_read(t, buf);
		if (k != T_BLOCKSIZE) {
			if (k != -1)
				errno = EINVAL;
			return -1;
		}
	}

	return 0;
}

/* hardlink */
int par_extract_hardlink(PAR * t, char *realname) {
	char *filename;
	char *linktgt = NULL;
	char *lnp;
	libtar_hashptr_t hp;

	if (!TH_ISLNK(t)) {
		errno = EINVAL;
		return -1;
	}

	filename = (realname ? realname : th_get_pathname(t));
	if (mkdirhier(dirname(filename)) == -1)
		return -1;
	libtar_hashptr_reset(&hp);
	if (libtar_hash_getkey(t->h, &hp, safer_name_suffix(th_get_linkname(t)),
			       (libtar_matchfunc_t)libtar_str_match) != 0) {
		lnp = (char *)libtar_hashptr_data(&hp);
		linktgt = &lnp[strlen(lnp) + 1];
	}
	else
		linktgt = safer_name_suffix(th_get_linkname(t));

#ifdef DEBUG
	printf("  ==> extracting: %s (link to %s)\n", filename, linktgt);
#endif
	if (link(linktgt, filename) == -1)
	{
#ifdef DEBUG
		perror("link()");
#endif
		return -1;
	}

	return 0;
}

/* symlink */
int par_extract_symlink(PAR *t, char *realname) {
	char *filename;

	if (!TH_ISSYM(t)) {
		errno = EINVAL;
		return -1;
	}

	filename = (realname ? realname : th_get_pathname(t));
	if (mkdirhier(dirname(filename)) == -1)
		return -1;

	if (unlink(filename) == -1 && errno != ENOENT)
		return -1;

#ifdef DEBUG
	printf("  ==> extracting: %s (symlink to %s)\n",
	       filename, safer_name_suffix(th_get_linkname(t)));
#endif
	if (symlink(safer_name_suffix(th_get_linkname(t)), filename) == -1) {
#ifdef DEBUG
		perror("symlink()");
#endif
		return -1;
	}

	return 0;
}

/* directory */
int par_extract_dir(PAR *t, char *realname) {
	mode_t mode;
	char *filename;

	if (!TH_ISDIR(t)) {
		errno = EINVAL;
		return -1;
	}

	filename = (realname ? realname : th_get_pathname(t));
	mode = th_get_mode(t);

	if (mkdirhier(dirname(filename)) == -1)
		return -1;

#ifdef DEBUG
	printf("  ==> extracting: %s (mode %04o, directory)\n", filename,
	       mode);
#endif
	if (mkdir(filename, mode) == -1) {
		if (errno == EEXIST) {
			if (chmod(filename, mode) == -1) {
#ifdef DEBUG
				perror("chmod()");
#endif
				return -1;
			} else {
#ifdef DEBUG
				puts("  *** using existing directory");
#endif
				return 1;
			}
		} else {
#ifdef DEBUG
			perror("mkdir()");
#endif
			return -1;
		}
	}

	return 0;
}
