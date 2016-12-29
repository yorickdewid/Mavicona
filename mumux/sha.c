/**
 * Copyright (C) 2015-2016 Mavicona, Quenza Inc.
 * All Rights Reserved
 *
 * This file is part of the Mavicona project.
 *
 * Content can not be copied and/or distributed without the express
 * permission of the author.
 */

#include "internal.h"
#include "sha.h"

#include <string.h>
#include <memory.h>

unsigned int wby_sha1_rol(unsigned int value, unsigned int bits) {
    return ((value) << bits) | (value >> (32 - bits));
}

void wby_sha1_hash_block(unsigned int state[5], const unsigned char *block) {
    int i;
    unsigned int a, b, c, d, e;
    unsigned int w[80];

    /* Prepare message schedule */
    for (i = 0; i < 16; ++i) {
        w[i] =  (((unsigned int)block[(i*4)+0]) << 24) |
                (((unsigned int)block[(i*4)+1]) << 16) |
                (((unsigned int)block[(i*4)+2]) <<  8) |
                (((unsigned int)block[(i*4)+3]) <<  0);
    }

    for (i = 16; i < 80; ++i)
        w[i] = wby_sha1_rol(w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16], 1);
    /* Initialize working variables */
    a = state[0]; b = state[1]; c = state[2]; d = state[3]; e = state[4];

    /* This is the core loop for each 20-word span. */
    #define SHA1_LOOP(start, end, func, constant) \
        for (i = (start); i < (end); ++i) { \
            unsigned int t = wby_sha1_rol(a, 5) + (func) + e + (constant) + w[i]; \
            e = d; d = c; c = wby_sha1_rol(b, 30); b = a; a = t;}

    SHA1_LOOP( 0, 20, ((b & c) ^ (~b & d)),           0x5a827999)
    SHA1_LOOP(20, 40, (b ^ c ^ d),                    0x6ed9eba1)
    SHA1_LOOP(40, 60, ((b & c) ^ (b & d) ^ (c & d)),  0x8f1bbcdc)
    SHA1_LOOP(60, 80, (b ^ c ^ d),                    0xca62c1d6)
    #undef SHA1_LOOP

    /* Update state */
    state[0] += a; state[1] += b; state[2] += c; state[3] += d; state[4] += e;
}

void wby_sha1_init(struct wby_sha1 *s) {
    s->state[0] = 0x67452301;
    s->state[1] = 0xefcdab89;
    s->state[2] = 0x98badcfe;
    s->state[3] = 0x10325476;
    s->state[4] = 0xc3d2e1f0;

    s->msg_size[0] = 0;
    s->msg_size[1] = 0;
    s->buf_used = 0;
}

void wby_sha1_update(struct wby_sha1 *s, const void *data_, unsigned long size) {
    const char *data = (const char*)data_;
    unsigned int size_lo;
    unsigned int size_lo_orig;
    unsigned long remain = size;

    while (remain > 0) {
        unsigned long buf_space = sizeof(s->buffer) - s->buf_used;
        unsigned long copy_size = (remain < buf_space) ? remain : buf_space;
        memcpy(s->buffer + s->buf_used, data, copy_size);

        s->buf_used += copy_size;
        data += copy_size;
        remain -= copy_size;

        if (s->buf_used == sizeof(s->buffer)) {
            wby_sha1_hash_block(s->state, s->buffer);
            s->buf_used = 0;
        }
    }

    size_lo = size_lo_orig = s->msg_size[1];
    size_lo += (unsigned int)(size * 8);
    if (size_lo < size_lo_orig)
        s->msg_size[0] += 1;
    s->msg_size[1] = size_lo;
}

void wby_sha1_final(unsigned char digest[20], struct wby_sha1 *s) {
    unsigned char zero = 0x00;
    unsigned char one_bit = 0x80;
    unsigned char count_data[8];
    int i;

    /* Generate size data in bit endian format */
    for (i = 0; i < 8; ++i) {
        unsigned int word = s->msg_size[i >> 2];
        count_data[i] = (unsigned char)(word >> ((3 - (i & 3)) * 8));
    }

    /* Set trailing one-bit */
    wby_sha1_update(s, &one_bit, 1);
    /* Emit null padding to to make room for 64 bits of size info in the last 512 bit block */
    while (s->buf_used != 56)
        wby_sha1_update(s, &zero, 1);

    /* Write size in bits as last 64-bits */
    wby_sha1_update(s, count_data, 8);
    /* Make sure we actually finalized our last block */
    WBY_ASSERT(s->buf_used == 0);

    /* Generate digest */
    for (i = 0; i < 20; ++i) {
        unsigned int word = s->state[i >> 2];
        unsigned char byte = (unsigned char) ((word >> ((3 - (i & 3)) * 8)) & 0xff);
        digest[i] = byte;
    }
}
