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

#ifndef OS_HPP
#define OS_HPP

#ifdef WIN32
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#else
#   include <unistd.h>
#   include <stdlib.h>
#   include <boost/filesystem.hpp>
using namespace boost::filesystem;
#endif
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <ups/upscaledb.h>
#include "1base/error.h"
#include "1base/util.h"

class os
{
protected:
#ifdef WIN32
  static const char *DisplayError(char* buf, uint32_t buflen, DWORD errorcode)
  {
    buf[0] = 0;
    FormatMessageA(/* FORMAT_MESSAGE_ALLOCATE_BUFFER | */
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, errorcode,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR)buf, buflen, NULL);
    buf[buflen - 1] = 0;
    return (buf);
  }
#endif

public:
  /*
   * delete a file
   */
  static bool unlink(const char *path, bool fail_silently = true)
  {
#ifdef WIN32
    BOOL rv = DeleteFileA((LPCSTR)path);
    if (!rv) {
      if (!fail_silently) {
        char buf[1024];
        char buf2[1024];
        DWORD st = GetLastError();
        _snprintf(buf2, sizeof(buf2),
                "DeleteFileA('%s') failed with OS status %u (%s)",
                path, st, DisplayError(buf, sizeof(buf), st));
        buf2[sizeof(buf2)-1] = 0;
        ups_log(("%s", buf2));
      }
      return (false);
    }
    return (true);
#else
    return (0 == ::unlink(path));
#endif
  }

  /*
   * copy a file
   */
  static bool copy(const char *src, const char *dest) {
#ifdef WIN32
    BOOL rv = CopyFileA((LPCSTR)src, dest, FALSE);
    if (!rv) {
      char buf[2048];
      char buf2[1024];
      DWORD st = GetLastError();
      _snprintf(buf2, sizeof(buf2),
            "CopyFileA('%s', '%s') failed with OS status %u (%s)",
            src, dest, st, DisplayError(buf, sizeof(buf), st));
      buf2[sizeof(buf2) - 1] = 0;
      ups_log(("%s", buf2));
      return (false);
    }
#else
    copy_file(path(src), path(dest), copy_option::overwrite_if_exists);
    //char buffer[1024 * 4];

    //snprintf(buffer, sizeof(buffer), "cp %s %s && chmod 644 %s",
            //src, dest, dest);
    //return (0 == system(buffer));
#endif
    return (true);
  }

  /*
   * check if a file exists
   */
  static bool file_exists(const char *path) {
#ifdef WIN32
    struct _stat buf = {0};
    if (::_stat(path, &buf) < 0)
      return (false);
#else
    struct stat buf = {0};
    if (::stat(path, &buf)<0)
      return (false);
#endif
    return (true);
  }
};

#endif /* OS_HPP */
