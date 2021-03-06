/**
 * Copyright (C) 2015-2016 Mavicona, Quenza Inc.
 * All Rights Reserved
 *
 * This file is part of the Mavicona project.
 *
 * Content can not be copied and/or distributed without the express
 * permission of the author.
 */

#ifndef ARC4RANDOM_H_INCLUDED
#define ARC4RANDOM_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#ifdef LINUX
void arc4random_stir();
void arc4random_addrandom(uint8_t *dat, int datlen);
uint32_t arc4random();
uint32_t arc4random_uniform(uint32_t range);
#endif // LINUX

#ifdef __cplusplus
}
#endif

#endif // ARC4RANDOM_H_INCLUDED

