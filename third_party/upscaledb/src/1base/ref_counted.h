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
 * A reference counted base class
 */

#ifndef UPS_REFERENCE_COUNTED_H
#define UPS_REFERENCE_COUNTED_H

#include "0root/root.h"

// Always verify that a file of level N does not include headers > N!

#ifndef UPS_ROOT_H
#  error "root.h was not included"
#endif

namespace upscaledb {

struct ReferenceCounted {
  ReferenceCounted()
    : refcounter(0) {
  }

  void add_ref() {
    refcounter++;
  }

  void release() {
    assert(refcounter > 0);
    refcounter--;
  }

  uint32_t refcounter;
};

} // namespace upscaledb

#endif /* UPS_REFERENCE_COUNTED_H */

