/**
 * Copyright (C) 2015-2016 Mavicona, Quenza Inc.
 * All Rights Reserved
 *
 * This file is part of the Mavicona project.
 *
 * Content can not be copied and/or distributed without the express
 * permission of the author.
 */

WBY_INTERN wby_size wby_url_decode(const char *src, wby_size src_len, char *dst, wby_size dst_len,
    int is_form_url_encoded) {
    int a, b;
    wby_size i, j;
    #define HEXTOI(x) (isdigit(x) ? x - '0' : x - 'W')
    for (i = j = 0; i < src_len && j < dst_len - 1; i++, j++) {
        if (src[i] == '%' &&
            isxdigit(*(const wby_byte*)(src + i + 1)) &&
            isxdigit(*(const wby_byte*)(src + i + 2)))
        {
            a = tolower(*(const wby_byte*)(src + i + 1));
            b = tolower(*(const wby_byte*)(src + i + 2));
            dst[j] = (char)((HEXTOI(a) << 4) | HEXTOI(b));
            i += 2;
        } else if (is_form_url_encoded && src[i] == '+') {
            dst[j] = ' ';
        } else dst[j] = src[i];
    }
    #undef HEXTOI
    dst[j] = '\0'; /* Null-terminate the destination */
    return j;
}

WBY_API int wby_find_query_var(const char *buf, const char *name, char *dst, wby_size dst_len) {
    const char *p, *e, *s;
    wby_size name_len;
    int len;
    wby_size buf_len = strlen(buf);

    name_len = strlen(name);
    e = buf + buf_len;
    len = -1;
    dst[0] = '\0';

    /* buf is "var1=val1&var2=val2...". Find variable first */
    for (p = buf; p != NULL && p + name_len < e; p++) {
        if ((p == buf || p[-1] == '&') && p[name_len] == '=' &&
            strncasecmp(name, p, name_len) == 0)
        {
            /* Point p to variable value */
            p += name_len + 1;
            /* Point s to the end of the value */
            s = (const char *) memchr(p, '&', (wby_size)(e - p));
            if (s == NULL) s = e;
            WBY_ASSERT(s >= p);
            /* Decode variable into destination buffer */
            if ((wby_size) (s - p) < dst_len)
                len = (int)wby_url_decode(p, (wby_size)(s - p), dst, dst_len, 1);
            break;
        }
    }
    return len;
}
