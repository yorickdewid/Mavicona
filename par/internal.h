/*
**  Copyright 1998-2003 University of Illinois Board of Trustees
**  Copyright 1998-2003 Mark D. Roth
**  Copyright 2015-2016 Mavicona, Quenza Inc.
**  All rights reserved.
**
**  internal.h - internal header file for libtar
**
**  This file is part of the Mavicona project.
**
**  Content can not be copied and/or distributed without the express
**  permission of the author.
*/

#ifndef INTERNAL_H
#define INTERNAL_H

#include <compat.h>

#ifdef DEBUG
# include <stdio.h>
# include <stdlib.h>
#endif

#include "libpar.h"

char* safer_name_suffix(char const*);

#endif

