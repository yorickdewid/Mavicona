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

/*
 * A simple wrapper around a file handle. Throws exceptions in
 * case of errors. Moves the file handle when copied.
 *
 * @exception_safe: strong
 * @thread_safe: unknown
 */

#ifndef UPS_FILE_H
#define UPS_FILE_H

#include "0root/root.h"
#include "1base/mutex.h"

#include <stdio.h>
#include <limits.h>

#include "ups/types.h"

// Always verify that a file of level N does not include headers > N!
#include "1os/os.h"

#ifndef UPS_ROOT_H
#  error "root.h was not included"
#endif

namespace upscaledb {

class File
{
  public:
    enum {
#ifdef UPS_OS_POSIX
      kSeekSet = SEEK_SET,
      kSeekEnd = SEEK_END,
      kSeekCur = SEEK_CUR,
      kMaxPath = PATH_MAX
#else
      kSeekSet = FILE_BEGIN,
      kSeekEnd = FILE_END,
      kSeekCur = FILE_CURRENT,
      kMaxPath = MAX_PATH
#endif
    };

    // Constructor: creates an empty File handle
    File()
      : m_fd(UPS_INVALID_FD), m_mmaph(UPS_INVALID_FD), m_posix_advice(0) {
    }

    // Copy constructor: moves ownership of the file handle
    File(File &other)
      : m_fd(other.m_fd), m_mmaph(other.m_mmaph),
        m_posix_advice(other.m_posix_advice) {
      other.m_fd = UPS_INVALID_FD;
	  other.m_mmaph = UPS_INVALID_FD;
    }

    // Move constructor: moves ownership of the file handle
    File(File &&other)
      : m_fd(other.m_fd), m_mmaph(other.m_mmaph),
        m_posix_advice(other.m_posix_advice) {
      other.m_fd = UPS_INVALID_FD;
      other.m_mmaph = UPS_INVALID_FD;
    }

    // Assignment operator: moves ownership of the file handle
    template<typename T> // can bind to lvalue AND rvalue
    File &operator=(T &&other) {
      m_fd = other.m_fd;
      other.m_fd = UPS_INVALID_FD;
      return *this;
    }

    // Destructor: closes the file
    ~File() {
      close();
    }

    // Creates a new file
    void create(const char *filename, uint32_t mode);

    // Opens an existing file
    void open(const char *filename, bool read_only);

    // Returns true if the file is open
    bool is_open() const {
      return m_fd != UPS_INVALID_FD;
    }

    // Flushes a file
    void flush();

    // Sets the parameter for posix_fadvise()
    void set_posix_advice(int parameter);

    // Maps a file in memory
    //
    // mmap is called with MAP_PRIVATE - the allocated buffer
    // is just a copy of the file; writing to the buffer will not alter
    // the file itself.
    void mmap(uint64_t position, size_t size, bool readonly,
                    uint8_t **buffer);

    // Unmaps a buffer
    void munmap(void *buffer, size_t size);

    // Positional read from a file
    void pread(uint64_t addr, void *buffer, size_t len);

    // Positional write to a file
    void pwrite(uint64_t addr, const void *buffer, size_t len);

    // Write data to a file; uses the current file position
    void write(const void *buffer, size_t len);

    // Get the page allocation granularity of the operating system
    static size_t granularity();

    // Seek position in a file
    void seek(uint64_t offset, int whence) const;

    // Tell the position in a file
    uint64_t tell() const;

    // Returns the size of the file
    uint64_t file_size() const;

    // Truncate/resize the file
    void truncate(uint64_t newsize);

    // Closes the file descriptor
    void close();

  private:
    // The file handle
    ups_fd_t m_fd;

    // The mmap handle - required for Win32
    ups_fd_t m_mmaph;

    // Parameter for posix_fadvise()
    int m_posix_advice;

#ifdef WIN32
	// A mutex; required for Win32
	Mutex m_mutex;
#endif
};

} // namespace upscaledb

#endif /* UPS_FILE_H */
