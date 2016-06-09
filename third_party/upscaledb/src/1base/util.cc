/*
 * Copyright (C) 2005-2016 Christoph Rupp (chris@crupp.de).
 * All Rights Reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * See the file COPYING for License information.
 */

// Always verify that a file of level N does not include headers > N!
#include "1base/util.h"

namespace upscaledb {

int
util_vsnprintf(char *str, size_t size, const char *format, va_list ap)
{
#if defined(UPS_OS_POSIX)
  return vsnprintf(str, size, format, ap);
#elif defined(UPS_OS_WIN32)
  return _vsnprintf(str, size, format, ap);
#else
  (void)size;
  return vsprintf(str, format, ap);
#endif
}

} // namespace upscaledb

