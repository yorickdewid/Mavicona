/* prototypes for borrowed "compatibility" code */

#include <sys/types.h>
#include <sys/stat.h>

#include <stdarg.h>
#include <stddef.h>

#ifdef HAVE_LIBGEN_H
# include <libgen.h>
#endif


# ifdef basename
#  undef basename		/* fix glibc brokenness */
# endif

char *openbsd_basename(const char *);
# define basename openbsd_basename





char *openbsd_dirname(const char *);
# define dirname openbsd_dirname



#ifdef NEED_GETHOSTBYNAME_R

# include <netdb.h>

# if GETHOSTBYNAME_R_NUM_ARGS != 6

int compat_gethostbyname_r(const char *, struct hostent *,
			   char *, size_t, struct hostent **, int *);

#  define gethostbyname_r compat_gethostbyname_r

# endif /* GETHOSTBYNAME_R_NUM_ARGS != 6 */

#endif /* NEED_GETHOSTBYNAME_R */




#if defined(NEED_GETHOSTNAME) && !defined(HAVE_GETHOSTNAME)

int gethostname(char *, size_t);

#endif /* NEED_GETHOSTNAME && ! HAVE_GETHOSTNAME */


#ifdef NEED_GETSERVBYNAME_R

# include <netdb.h>

# if GETSERVBYNAME_R_NUM_ARGS != 6

int compat_getservbyname_r(const char *, const char *, struct servent *,
			   char *, size_t, struct servent **);

#  define getservbyname_r compat_getservbyname_r

# endif /* GETSERVBYNAME_R_NUM_ARGS != 6 */

#endif /* NEED_GETSERVBYNAME_R */


#if defined(NEED_INET_ATON) && !defined(HAVE_INET_ATON)

int inet_aton(const char *, struct in_addr *);

#endif /* NEED_INET_ATON && ! HAVE_INET_ATON */


#ifdef NEED_MAKEDEV

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

#endif /* NEED_MAKEDEV */


#if defined(NEED_SNPRINTF) && !defined(HAVE_SNPRINTF)

int mutt_snprintf(char *, size_t, const char *, ...);
int mutt_vsnprintf(char *, size_t, const char *, va_list);
#define snprintf mutt_snprintf
#define vsnprintf mutt_vsnprintf

#endif /* NEED_SNPRINTF && ! HAVE_SNPRINTF */




size_t strlcat(char *, const char *, size_t);


size_t strlcpy(char *, const char *, size_t);


#if defined(NEED_STRDUP) && !defined(HAVE_STRDUP)

char *openbsd_strdup(const char *);
# define strdup openbsd_strdup

#endif /* NEED_STRDUP && ! HAVE_STRDUP */



void strmode(register mode_t, register char *);



#if defined(NEED_STRRSTR) && !defined(HAVE_STRRSTR)

char *strrstr(char *, char *);

#endif /* NEED_STRRSTR && ! HAVE_STRRSTR */


#ifdef NEED_STRSEP

# ifdef HAVE_STRSEP
#  define _LINUX_SOURCE_COMPAT		/* needed on AIX 4.3.3 */
# else

char *strsep(register char **, register const char *);

# endif

#endif /* NEED_STRSEP */


