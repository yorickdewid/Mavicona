/**
 * Copyright (C) 2015-2016 Mavicona, Quenza Inc.
 * All Rights Reserved
 *
 * This file is part of the Mavicona project.
 *
 * Content can not be copied and/or distributed without the express
 * permission of the author.
 */

#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <unistd.h>

#define PNULL	(void **)0

#define O_BINARY 0

#define TRUE    1
#define FALSE   0

#define NOLOCK 0x0
#define LOCK 0x1

#define RSIZE(e) sizeof(e)/sizeof(e[0])
#define unused(v) (void)(v)

#define _IN_
#define _OUT_

#define LOGFILE			"quid.log"

#ifdef LINUX
size_t strlcpy(char *dst, const char *src, size_t siz);
size_t strlcat(char *dst, const char *src, size_t siz);
#endif // LINUX

#if defined(__mc68000__) || defined (__sparc__) || defined (__PPC__) \
    || (defined(__mips__) && (defined(MIPSEB) || defined (__MIPSEB__)) ) \
    || defined(__hpux__)  /* should be replaced by the macro for the PA */
#define BIG_ENDIAN_HOST 1
#else
#define LITTLE_ENDIAN_HOST 1
#endif

#define zassert(e)  \
	((void) ((e) ? (void)0 : __zassert (#e, __FILE__, __LINE__)))

#define __zassert(e, file, line) \
	((void)printf("%s:%u: failed assertion `%s'\n", file, line, e), abort())

#define nullify(p, sz) \
	memset(p, 0, sz)

char from_hex(char ch);
int8_t strisbool(char *str);
char *strtolower(char *str);
char *strtoupper(char *str);
char *strdtrim(char *str);
char *strtoken(char *s, const char *delim);
int strccnt(const char *str, char c);
int antoi(const char *str, size_t num);
char *itoa(long i);
char *strdup(const char *str);
char *strndup(const char *str, size_t n);
char *stresc(char *src, size_t *_len);
char *strsep(char ** stringp, const char *delim);

#ifdef __cplusplus
}
#endif

#endif // COMMON_H_INCLUDED
