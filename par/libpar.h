/*
**  Copyright 1998-2003 University of Illinois Board of Trustees
**  Copyright 1998-2003 Mark D. Roth
**  Copyright 2015-2016 Mavicona, Quenza Inc.
**  All rights reserved.
**
**  libpar.h - header file for libtar library
**
**  This file is part of the Mavicona project.
**
**  Content can not be copied and/or distributed without the express
**  permission of the author.
*/

#ifndef LIBPAR_H
#define LIBPAR_H

#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
// #include <tar.h>

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
#define PACKAGE_MAGIC "$MAVJOB}"

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
// #define AREGTYPE	0x10

/* our version of the tar header structure */
struct archive_header {
	char magic[8];
	uint8_t version_major;
	uint8_t version_minor;
	uint8_t options;
	char quid[36];
};

/* our version of the par header structure */
struct tar_header {
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
	char devmajor[8];
	char devminor[8];
	char prefix[155];
	char padding[12];
	char *gnu_longname;
	char *gnu_longlink;
};

/***** handle.c ************************************************************/

typedef int (*openfunc_t)(const char *, int, ...);
typedef int (*closefunc_t)(int);
typedef ssize_t (*readfunc_t)(int, void *, size_t);
typedef ssize_t (*writefunc_t)(int, const void *, size_t);

typedef struct {
	openfunc_t openfunc;
	closefunc_t closefunc;
	readfunc_t readfunc;
	writefunc_t writefunc;
} partype_t;

typedef struct {
	partype_t *type;
	const char *pathname;
	long fd;
	int oflags;
	int options;
	struct tar_header th_buf;
	libtar_hash_t *h;
	char *th_pathname;
} PAR;

/* constant values for the PAR options field */
#define PAR_GNU			 1	/* use GNU extensions */
#define PAR_VERBOSE		 2	/* output file info to stdout */
#define PAR_NOOVERWRITE		 4	/* don't overwrite existing files */
#define PAR_IGNORE_EOT		 8	/* ignore double zero blocks as EOF */
#define PAR_CHECK_MAGIC		16	/* check magic in file header */
#define PAR_CHECK_VERSION	32	/* check version in file header */
#define PAR_IGNORE_CRC		64	/* ignore CRC in file header */

/* open a new tarfile handle */
int par_open(PAR **t, const char *pathname, partype_t *type,
	     int oflags, int mode, int options);

/* close tarfile handle */
int par_close(PAR *t);


/***** append.c ************************************************************/

/* forward declaration to appease the compiler */
struct tar_dev;

/* cleanup function */
void tar_dev_free(struct tar_dev *tdp);

/* Appends a file to the tar archive.
 * Arguments:
 *    t        = PAR handle to append to
 *    realname = path of file to append
 *    savename = name to save the file under in the archive
 */
int par_append_file(PAR *t, const char *realname, const char *savename);

/* write EOF indicator */
int par_append_eof(PAR *t);

/* add file contents to a tarchive */
int par_append_regfile(PAR *t, const char *realname);


/***** block.c *************************************************************/

/* macros for reading/writing tarchive blocks */
#define par_block_read(t, buf) \
	(*((t)->type->readfunc))((t)->fd, (char *)(buf), T_BLOCKSIZE)
#define par_block_write(t, buf) \
	(*((t)->type->writefunc))((t)->fd, (char *)(buf), T_BLOCKSIZE)

/* read/write a header block */
int th_read(PAR *t);
int th_write(PAR *t);


/***** decode.c ************************************************************/

/* determine file type */
// #define TH_ISREG(t)	((t)->th_buf.typeflag == REGTYPE \
// 			 || (t)->th_buf.typeflag == AREGTYPE \
// 			 || (t)->th_buf.typeflag == CONTTYPE \
// 			 || (S_ISREG((mode_t)oct_to_int((t)->th_buf.mode)) \
// 			     && (t)->th_buf.typeflag != LNKTYPE))
#define TH_ISREG(t)	((t)->th_buf.typeflag == REGTYPE \
			 || (S_ISREG((mode_t)oct_to_int((t)->th_buf.mode)) \
			     && (t)->th_buf.typeflag != LNKTYPE))

#define TH_ISLNK(t)	((t)->th_buf.typeflag == LNKTYPE)
#define TH_ISSYM(t)	((t)->th_buf.typeflag == SYMTYPE \
			 || S_ISLNK((mode_t)oct_to_int((t)->th_buf.mode)))
// #define TH_ISCHR(t)	((t)->th_buf.typeflag == CHRTYPE \
// 			 || S_ISCHR((mode_t)oct_to_int((t)->th_buf.mode)))
// #define TH_ISBLK(t)	((t)->th_buf.typeflag == BLKTYPE \
// 			 || S_ISBLK((mode_t)oct_to_int((t)->th_buf.mode)))
// #define TH_ISDIR(t)	((t)->th_buf.typeflag == DIRTYPE \
// 			 || S_ISDIR((mode_t)oct_to_int((t)->th_buf.mode)) \
// 			 || ((t)->th_buf.typeflag == AREGTYPE \
// 			     && strlen((t)->th_buf.name) \
// 			     && ((t)->th_buf.name[strlen((t)->th_buf.name) - 1] == '/')))

#define TH_ISDIR(t)	((t)->th_buf.typeflag == DIRTYPE \
			 || S_ISDIR((mode_t)oct_to_int((t)->th_buf.mode)))

// #define TH_ISFIFO(t)	((t)->th_buf.typeflag == FIFOTYPE \
// 			 || S_ISFIFO((mode_t)oct_to_int((t)->th_buf.mode)))
#define TH_ISLONGNAME(t)	((t)->th_buf.typeflag == GNU_LONGNAME_TYPE)
#define TH_ISLONGLINK(t)	((t)->th_buf.typeflag == GNU_LONGLINK_TYPE)

/* decode tar header info */
#define th_get_crc(t) oct_to_int((t)->th_buf.chksum)
#define th_get_size(t) oct_to_size((t)->th_buf.size)
#define th_get_mtime(t) oct_to_int((t)->th_buf.mtime)
#define th_get_devmajor(t) oct_to_int((t)->th_buf.devmajor)
#define th_get_devminor(t) oct_to_int((t)->th_buf.devminor)
#define th_get_linkname(t) ((t)->th_buf.gnu_longlink \
                            ? (t)->th_buf.gnu_longlink \
                            : (t)->th_buf.linkname)
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

/* encode magic, version, and crc - must be done after everything else is set */
void th_finish(PAR *t);


/***** extract.c ***********************************************************/

/* sequentially extract next file from t */
int par_extract_file(PAR *t, char *realname);

/* extract different file types */
int par_extract_dir(PAR *t, char *realname);
int par_extract_hardlink(PAR *t, char *realname);
int par_extract_symlink(PAR *t, char *realname);
// int par_extract_chardev(PAR *t, char *realname);
// int par_extract_blockdev(PAR *t, char *realname);
// int par_extract_fifo(PAR *t, char *realname);

/* for regfiles, we need to extract the content blocks as well */
int par_extract_regfile(PAR *t, char *realname);
int par_skip_regfile(PAR *t);


/***** output.c ************************************************************/

/* print the tar header */
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
