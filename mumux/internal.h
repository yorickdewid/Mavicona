#ifndef MUMUX_INTERNAL_H
#define MUMUX_INTERNAL_H

#include "libmumux.h"

#ifdef WBY_USE_ASSERT
#ifndef WBY_ASSERT
#include <assert.h>
#define WBY_ASSERT(expr) assert(expr)
#endif
#else
#define WBY_ASSERT(expr)
#endif

typedef int wby__check_ptr_size[(sizeof(void*) == sizeof(unsigned long)) ? 1 : -1];
#define WBY_LEN(a) (sizeof(a)/sizeof((a)[0]))
#define WBY_UNUSED(a) ((void)(a))

#if defined(__GNUC__)
#define WBY_ALIGN(x) __attribute__((aligned(x)))
#else
#define WBY_ALIGN(x) __declspec(align(x))
#endif

/* URL */

unsigned long wby_url_decode(const char *src, unsigned long src_len, char *dst, unsigned long dst_len,
    int is_form_url_encoded);

int wby_find_query_var(const char *buf, const char *name, char *dst, unsigned long dst_len);

#endif // MUMUX_INTERNAL_H
