/*
**  Copyright 1998-2003 University of Illinois Board of Trustees
**  Copyright 1998-2003 Mark D. Roth
**  Copyright 2015-2016 Mavicona, Quenza Inc.
**  All rights reserved.
**
**  libpar.h - header file for libpar library
**
**  This file is part of the Mavicona project.
**
**  Content can not be copied and/or distributed without the express
**  permission of the author.
*/

#ifndef LIBPAR_H
#define LIBPAR_H

#if defined(LINUX)
# ifndef _DEFAULT_SOURCE
#  define _DEFAULT_SOURCE
# endif
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <quid.h>
#include <zlib.h>

#include "libtar_listhash.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.2"
#define PACKAGE_VERSION_MAJOR 1
#define PACKAGE_VERSION_MINOR 2

/* Package magic. */
#define PACKAGE_MAGIC "$MAVJO}"

/* par header info */
#define TVERSION	"MY"
#define TVERSLEN	2
#define TMAGIC		"^*TAd&"
#define TMAGLEN		6

/* useful constants */
/* see FIXME note in block.c regarding T_BLOCKSIZE */
#define T_BLOCKSIZE		512
#define T_NAMELEN		100
#define T_PREFIXLEN		155
#define T_MAXPATHLEN		(T_NAMELEN + T_PREFIXLEN)

/* GNU extensions for typeflag */
#define GNU_LONGNAME_TYPE	'L'
#define GNU_LONGLINK_TYPE	'K'

/* file type definitions */
#define SYMTYPE		0x1
#define REGTYPE		0x2
#define DIRTYPE		0x4
#define LNKTYPE		0x8

/* package meta */
typedef struct par_meta {
	char name[64];
} par_meta_t;

/* our version of the header structure */
struct archive_header {
	char magic[8];
	uint8_t version_major;
	uint8_t version_minor;
	uint8_t options;
	quid_t quid;
	par_meta_t meta;
};

/* our version of the par header structure */
struct par_file_header {
	char name[100];
	char mode[8];
	char uid[8];
	char gid[8];
	char size[12];
	char mtime[12];
	char chksum[8];
	char typeflag;
	char linkname[100];
	char magic[6];
	char version[2];
	char uname[32];
	char gname[32];
	char prefix[155];
	char padding[12];
	char *gnu_longname;
	char *gnu_longlink;
};

/***** handle.c ************************************************************/

typedef struct {
	const char *pathname;
	int fd;
	gzFile gzfd;
	int oflags;
	int options;
	int use_gz;
	struct par_file_header th_buf;
	libtar_hash_t *h;
	char *th_pathname;
} PAR;

/* constant values for the PAR options field */
#define PAR_GNU			 1	/* use GNU extensions */
#define PAR_VERBOSE		 2	/* output file info to stdout */
#define PAR_NOOVERWRITE		 4	/* don't overwrite existing files */
#define PAR_IGNORE_EOT		 8	/* ignore double zero blocks as EOF */
#define PAR_COMPRESS		16	/* compress archive */

/**/
int par_block_read(PAR *t, char *buf);

/**/
int par_block_write(PAR *t, char *buf);

/**/
int par_write_header(PAR *t);

/**/
int par_read_header(PAR *t);

/* open a new file handle */
int par_open(PAR **t, const char *pathname, int compress,
	     int oflags, int mode, int options);

/* close file handle */
int par_close(PAR *t);


/***** append.c ************************************************************/

/* forward declaration to appease the compiler */
struct tar_dev;

/* cleanup function */
void tar_dev_free(struct tar_dev *tdp);

/* Appends a file to the archive.
 * Arguments:
 *    t        = PAR handle to append to
 *    realname = path of file to append
 *    savename = name to save the file under in the archive
 */
int par_append_file(PAR *t, const char *realname, const char *savename);

/* write EOF indicator */
int par_append_eof(PAR *t);

/* add file contents to a archive */
int par_append_regfile(PAR *t, const char *realname);


/***** block.c *************************************************************/

/* read/write a header block */
int th_read(PAR *t);
int th_write(PAR *t);

/***** decode.c ************************************************************/

/* determine file type */
#define TH_ISREG(t)	((t)->th_buf.typeflag == REGTYPE \
			 || (S_ISREG((mode_t)oct_to_int((t)->th_buf.mode)) \
			     && (t)->th_buf.typeflag != LNKTYPE))
#define TH_ISLNK(t)	((t)->th_buf.typeflag == LNKTYPE)
#define TH_ISSYM(t)	((t)->th_buf.typeflag == SYMTYPE \
			 || S_ISLNK((mode_t)oct_to_int((t)->th_buf.mode)))
#define TH_ISDIR(t)	((t)->th_buf.typeflag == DIRTYPE \
			 || S_ISDIR((mode_t)oct_to_int((t)->th_buf.mode)))
#define TH_ISLONGNAME(t)	((t)->th_buf.typeflag == GNU_LONGNAME_TYPE)
#define TH_ISLONGLINK(t)	((t)->th_buf.typeflag == GNU_LONGLINK_TYPE)

/* decode header info */
#define th_get_crc(t) oct_to_int((t)->th_buf.chksum)
#define th_get_size(t) oct_to_size((t)->th_buf.size)
#define th_get_mtime(t) oct_to_int((t)->th_buf.mtime)
#define th_get_linkname(t) ((t)->th_buf.gnu_longlink \
                            ? (t)->th_buf.gnu_longlink \
                            : (t)->th_buf.linkname)
char *safer_name_suffix(char const *file_name);
char *th_get_pathname(PAR *t);
mode_t th_get_mode(PAR *t);
uid_t th_get_uid(PAR *t);
gid_t th_get_gid(PAR *t);

/***** encode.c ************************************************************/

/* encode file info in th_header */
void th_set_type(PAR *t, mode_t mode);
void th_set_path(PAR *t, const char *pathname);
void th_set_link(PAR *t, const char *linkname);
// void th_set_device(PAR *t, dev_t device);
void th_set_user(PAR *t, uid_t uid);
void th_set_group(PAR *t, gid_t gid);
void th_set_mode(PAR *t, mode_t fmode);
#define th_set_mtime(t, fmtime) \
	int_to_oct_nonull((fmtime), (t)->th_buf.mtime, 12)
#define th_set_size(t, fsize) \
	int_to_oct_nonull((fsize), (t)->th_buf.size, 12)

/* encode everything at once (except the pathname and linkname) */
int th_set_from_stat(PAR *t, struct stat *s);

/***** extract.c ***********************************************************/

/* sequentially extract next file from t */
int par_extract_file(PAR *t, char *realname);

/* extract different file types */
int par_extract_dir(PAR *t, char *realname);
int par_extract_hardlink(PAR *t, char *realname);
int par_extract_symlink(PAR *t, char *realname);

/* for regfiles, we need to extract the content blocks as well */
int par_extract_regfile(PAR *t, char *realname);
int par_skip_regfile(PAR *t);

/***** output.c ************************************************************/

/* print the header */
void th_print(PAR *t);

/* print "ls -l"-like output for the file described by th */
void th_print_long_ls(PAR *t);

/***** util.c *************************************************************/

/* hashing function for pathnames */
int path_hashfunc(char *key, int numbuckets);

/* matching function for dev_t's */
int dev_match(dev_t *dev1, dev_t *dev2);

/* matching function for ino_t's */
int ino_match(ino_t *ino1, ino_t *ino2);

/* hashing function for dev_t's */
int dev_hash(dev_t *dev);

/* hashing function for ino_t's */
int ino_hash(ino_t *inode);

/* create any necessary dirs */
int mkdirhier(char *path);

/* calculate header checksum */
int th_crc_calc(PAR *t);

/* calculate a signed header checksum */
int th_signed_crc_calc(PAR *t);

/* compare checksums in a forgiving way */
#define th_crc_ok(t) (th_get_crc(t) == th_crc_calc(t) || th_get_crc(t) == th_signed_crc_calc(t))

/* string-octal to integer conversion */
int oct_to_int(char *oct);
size_t oct_to_size(char *oct);

/* integer to NULL-terminated string-octal conversion */
#define int_to_oct(num, oct, octlen) \
	snprintf((oct), (octlen), "%*lo ", (octlen) - 2, (unsigned long)(num))

/* integer to string-octal conversion, no NULL */
void int_to_oct_nonull(int num, char *oct, size_t octlen);

/***** wrapper.c **********************************************************/

/* extract groups of files */
int par_extract_glob(PAR *t, char *globname, char *prefix);
int par_extract_all(PAR *t, char *prefix);

/* add a whole tree of files */
int par_append_tree(PAR *t, char *realdir, char *savedir);


#ifdef __cplusplus
}
#endif

#endif /* ! LIBPAR_H */
