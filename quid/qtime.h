/**
 * Copyright (C) 2015-2016 Mavicona, Quenza Inc.
 * All Rights Reserved
 *
 * This file is part of the Mavicona project.
 *
 * Content can not be copied and/or distributed without the express
 * permission of the author.
 */
#ifndef TIME_H_INCLUDED
#define TIME_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

#include <time.h>

#define TIMENAME_SIZE	12
#define ISO_8601_FORMAT	"%F %T"
#define US_FORMAT		"%d/%m/%Y %H:%M:%S %z"

long long get_timestamp();
long long get_unixtimestamp();
char *tstostrf(char *buf, size_t len, long long ts, char *fmt);
char *unixtostrf(char *buf, size_t len, long long ts, char *fmt);
long long timetots(struct tm *t);
char *timename_now(char *str);

#ifdef __cplusplus
}
#endif

#endif // TIME_H_INCLUDED
