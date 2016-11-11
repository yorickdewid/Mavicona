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

#ifndef UPS_TXN_REMOTE_H
#define UPS_TXN_REMOTE_H

#ifdef UPS_ENABLE_REMOTE

#include "0root/root.h"

// Always verify that a file of level N does not include headers > N!
#include "4txn/txn.h"

#ifndef UPS_ROOT_H
#  error "root.h was not included"
#endif

namespace upscaledb {

struct Context;

//
// A remote Txn
//
struct RemoteTxn : Txn {
  // Constructor; "begins" the Txn
  // supported flags: UPS_TXN_READ_ONLY, UPS_TXN_TEMPORARY
  RemoteTxn(Env *env, const char *name, uint32_t flags, uint64_t remote_handle);

  // Commits the Txn
  virtual void commit();

  // Aborts the Txn
  virtual void abort();

  // The remote Txn handle
  uint64_t remote_handle;
};


//
// A TxnManager for remote Txns
//
struct RemoteTxnManager : TxnManager {
  // Constructor
  RemoteTxnManager(Env *env)
    : TxnManager(env) {
  }

  // Begins a new Txn
  virtual void begin(Txn *txn);

  // Commits a Txn; the derived subclass has to take care of
  // flushing and/or releasing memory
  virtual ups_status_t commit(Txn *txn);

  // Aborts a Txn; the derived subclass has to take care of
  // flushing and/or releasing memory
  virtual ups_status_t abort(Txn *txn);

  // Flushes committed (queued) transactions
  virtual void flush_committed_txns(Context *context = 0);
};

} // namespace upscaledb

#endif // UPS_ENABLE_REMOTE

#endif /* UPS_TXN_REMOTE_H */
