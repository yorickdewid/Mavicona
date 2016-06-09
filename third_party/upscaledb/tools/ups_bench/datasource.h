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

#ifndef UPS_BENCH_DATASOURCE_H
#define UPS_BENCH_DATASOURCE_H

#include <vector>

//
// abstract base class for a data source - generates test data
//
struct Datasource
{
  // virtual destructor - can be overwritten
  virtual ~Datasource() {
  }

  // resets the input and restarts delivering the same sequence
  // from scratch
  virtual void reset() = 0;

  // returns the next piece of data
  virtual void next(std::vector<uint8_t> &vec) = 0;
};

#endif /* UPS_BENCH_DATASOURCE_H */
