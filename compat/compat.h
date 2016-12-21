/**
 * Copyright (C) 2015-2016 Mavicona, Quenza Inc.
 * All Rights Reserved
 *
 * This file is part of the Mavicona project.
 *
 * Content can not be copied and/or distributed without the express
 * permission of the author.
 */

#include <sys/types.h>
#include <sys/stat.h>

#include <stdarg.h>
#include <stddef.h>
#include <libgen.h>

#ifndef MAXPATHLEN
#define MAXPATHLEN 255
#endif

#define UNUSED(x) (void)(x)

#ifdef __cplusplus
extern "C" {
#endif

/*
 * basename
 */

#if defined(OS_UNKNOWN)
# ifdef basename
#  undef basename
# endif

char *compat_basename(const char *);
# define basename compat_basename

#endif // OS_UNKNOWN


/*
 * dirname
 */

#if defined(OS_UNKNOWN)
# ifdef dirname
#  undef dirname
# endif

char *compat_dirname(const char *);
# define dirname compat_dirname

#endif // OS_UNKNOWN


/*
 * gethostbyname_r
 */

#if defined(OS_UNKNOWN)
# include <netdb.h>

int compat_gethostbyname_r(const char *, struct hostent *,
			   char *, size_t, struct hostent **, int *);
# define gethostbyname_r compat_gethostbyname_r

#endif // OS_UNKNOWN


/*
 * gethostname
 */

#if defined(OS_UNKNOWN)
# ifdef gethostname
#  undef gethostname
# endif

int gethostname(char *, size_t);

#endif // OS_UNKNOWN


/*
 * getservbyname_r
 */

#if defined(OS_UNKNOWN)
# include <netdb.h>

int compat_getservbyname_r(const char *, const char *, struct servent *,
			   char *, size_t, struct servent **);
# define getservbyname_r compat_getservbyname_r

#endif // OS_UNKNOWN


/*
 * inet_aton
 */

#if defined(OS_UNKNOWN)

int inet_aton(const char *, struct in_addr *);

#endif // OS_UNKNOWN


/*
 * makedev
 */

#if defined(OS_UNKNOWN)

# ifdef MAJOR_IN_MKDEV
#  include <sys/mkdev.h>
# else
#  ifdef MAJOR_IN_SYSMACROS
#   include <sys/sysmacros.h>
#  endif
# endif

/*
** On most systems makedev() has two args.
** Some weird systems, like QNX6, have makedev() functions that expect
** an extra first argument for "node", which can be 0 for a local
** machine.
*/

# ifdef MAKEDEV_THREE_ARGS
#  define compat_makedev(maj, min)	makedev(0, maj, min)
# else
#  define compat_makedev		makedev
# endif

#endif // OS_UNKNOWN


/*
 * snprintf
 */

#if defined(OS_UNKNOWN)

int mutt_snprintf(char *, size_t, const char *, ...);
int mutt_vsnprintf(char *, size_t, const char *, va_list);
#define snprintf mutt_snprintf
#define vsnprintf mutt_vsnprintf

#endif // OS_UNKNOWN


/*
 * mkdirhier
 */
#if defined(OS_UNKNOWN)

int mkdirhier(char *path);

#endif // OS_UNKNOWN


/*
 * strlcat / strlcpy
 */

#if defined(LINUX)

size_t strlcat(char *, const char *, size_t);
size_t strlcpy(char *, const char *, size_t);

#endif // LINUX


/*
 * strdup
 */

#if defined(LINUX)

char *compat_strdup(const char *);
# define strdup compat_strdup

#endif // LINUX


/*
 * strmode
 */

#if defined(LINUX)

void strmode(register mode_t, register char *);

#endif // LINUX


/*
 * strrstr
 */

#if defined(LINUX)

char *strrstr(char *, char *);

#endif // LINUX


/*
 * strsep
 */

#if defined(LINUX)
# ifdef strsep
#  undef strsep
# endif

char *compat_strsep(register char **, register const char *);
# define strsep compat_strsep

#endif // LINUX


/*
 * getopt
 */

#if defined(OS_UNKNOWN)

int getopt(int argc, char *const *argv, const char *optstring);

#endif // OS_UNKNOWN


#ifdef __cplusplus
}
#endif
