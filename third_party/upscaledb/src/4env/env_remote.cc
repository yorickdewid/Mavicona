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

#ifdef UPS_ENABLE_REMOTE

#include "0root/root.h"

// Always verify that a file of level N does not include headers > N!
#include "1os/os.h"
#include "1base/scoped_ptr.h"
#include "2protobuf/protocol.h"
#include "4cursor/cursor.h"
#include "4cursor/cursor_remote.h"
#include "4db/db_remote.h"
#include "4env/env_remote.h"
#include "4txn/txn_remote.h"
#include "4uqi/result.h"

#ifndef UPS_ROOT_H
#  error "root.h was not included"
#endif

namespace upscaledb {

RemoteEnvironment::RemoteEnvironment(EnvConfig config)
  : Environment(config), m_remote_handle(0), m_buffer(1024 * 4)
{
}

Protocol *
RemoteEnvironment::perform_request(Protocol *request)
{
  // use ByteArray to avoid frequent reallocs!
  m_buffer.clear();

  if (!request->pack(&m_buffer)) {
    ups_log(("protoype Protocol::pack failed"));
    throw Exception(UPS_INTERNAL_ERROR);
  }

  m_socket.send(m_buffer.data(), m_buffer.size());

  // now block and wait for the reply; first read the header, then the
  // remaining data
  m_socket.recv(m_buffer.data(), 8);

  // no need to check the magic; it's verified in Protocol::unpack
  uint32_t size = *(uint32_t *)(m_buffer.data() + 4);
  m_buffer.resize(size + 8);
  m_socket.recv(m_buffer.data() + 8, size);

  return (Protocol::unpack(m_buffer.data(), size + 8));
}

void
RemoteEnvironment::perform_request(SerializedWrapper *request,
                SerializedWrapper *reply)
{
  int size_left = (int)request->get_size();
  request->size = size_left;
  request->magic = UPS_TRANSFER_MAGIC_V2;
  m_buffer.resize(request->size);

  uint8_t *ptr = m_buffer.data();
  request->serialize(&ptr, &size_left);
  assert(size_left == 0);

  m_socket.send(m_buffer.data(), request->size);

  // now block and wait for the reply; first read the header, then the
  // remaining data
  m_socket.recv(m_buffer.data(), 8);

  // now check the magic and receive the remaining data
  uint32_t magic = *(uint32_t *)(m_buffer.data() + 0);
  if (magic != UPS_TRANSFER_MAGIC_V2)
    throw Exception(UPS_INTERNAL_ERROR);
  // TODO check the magic
  int size = *(int *)(m_buffer.data() + 4);
  m_buffer.resize(size);
  m_socket.recv(m_buffer.data() + 8, size - 8);

  ptr = m_buffer.data();
  reply->deserialize(&ptr, &size);
  assert(size == 0);
}

template<typename T>
static void
append_array(std::vector<T> &v, T *data, size_t size)
{
  v.insert(v.end(), data, data + size);
}

ups_status_t
RemoteEnvironment::select_range(const char *query, Cursor *begin,
                            const Cursor *end, Result **presult)
{
  Protocol request(Protocol::SELECT_RANGE_REQUEST);
  request.mutable_select_range_request();
  request.mutable_select_range_request()->set_env_handle(m_remote_handle);
  request.mutable_select_range_request()->set_query(query);
  if (begin) {
    RemoteCursor *c = (RemoteCursor *)begin;
    request.mutable_select_range_request()->set_begin_cursor_handle(c->remote_handle());
  }
  if (end) {
    RemoteCursor *c = (RemoteCursor *)end;
    request.mutable_select_range_request()->set_end_cursor_handle(c->remote_handle());
  }

  ScopedPtr<Protocol> reply(perform_request(&request));

  assert(reply->has_select_range_reply());

  ups_status_t st = reply->select_range_reply().status();
  if (st)
    return (st);

  /* copy the result */
  Result *r = new Result;
  r->row_count = reply->select_range_reply().row_count();
  r->key_type = reply->select_range_reply().key_type();
  append_array(r->key_offsets,
                  (uint32_t *)reply->select_range_reply().key_offsets().begin(),
                  reply->select_range_reply().key_offsets().size());
  append_array(r->key_data, 
                  (uint8_t *)&reply->select_range_reply().key_data()[0],
                  reply->select_range_reply().key_data().size());
  r->record_type = reply->select_range_reply().record_type();
  append_array(r->record_offsets,
                  (uint32_t *)reply->select_range_reply().record_offsets().begin(),
                  reply->select_range_reply().record_offsets().size());
  append_array(r->record_data,
                  (uint8_t *)&reply->select_range_reply().record_data()[0],
                  reply->select_range_reply().record_data().size());
  *presult = r;

  return (0);
}

ups_status_t
RemoteEnvironment::do_create()
{
  // the 'create' operation is identical to 'open'
  return (do_open());
}

ups_status_t
RemoteEnvironment::do_open()
{
  m_socket.close();

  const char *url = m_config.filename.c_str();
  assert(url != 0);
  assert(::strstr(url, "ups://") == url);
  const char *ip = url + 6;
  const char *port_str = strstr(ip, ":");
  if (!port_str) {
    ups_trace(("remote uri does not include port - expected "
                "`ups://<ip>:<port>`"));
    return (UPS_INV_PARAMETER);
  }
  uint16_t port = (uint16_t)atoi(port_str + 1);
  if (!port) {
    ups_trace(("remote uri includes invalid port - expected "
                "`ups://<ip>:<port>`"));
    return (UPS_INV_PARAMETER);
  }

  const char *filename = strstr(port_str, "/");

  std::string hostname(ip, port_str);
  m_socket.connect(hostname.c_str(), port, m_config.remote_timeout_sec);

  Protocol request(Protocol::CONNECT_REQUEST);
  request.mutable_connect_request()->set_path(filename);

  ScopedPtr<Protocol> reply(perform_request(&request));

  assert(reply->type() == Protocol::CONNECT_REPLY);

  ups_status_t st = reply->connect_reply().status();
  if (st == 0) {
    m_config.flags |= reply->connect_reply().env_flags();
    m_remote_handle = reply->connect_reply().env_handle();

    if (get_flags() & UPS_ENABLE_TRANSACTIONS)
      m_txn_manager.reset(new RemoteTransactionManager(this));
  }

  return (st);
}

ups_status_t
RemoteEnvironment::do_get_database_names(uint16_t *names, uint32_t *count)
{
  Protocol request(Protocol::ENV_GET_DATABASE_NAMES_REQUEST);
  request.mutable_env_get_database_names_request();
  request.mutable_env_get_database_names_request()->set_env_handle(m_remote_handle);

  ScopedPtr<Protocol> reply(perform_request(&request));

  assert(reply->has_env_get_database_names_reply());

  ups_status_t st = reply->env_get_database_names_reply().status();
  if (st)
    return (st);

  /* copy the retrieved names */
  uint32_t i;
  for (i = 0;
      i < (uint32_t)reply->env_get_database_names_reply().names_size()
        && i < *count;
      i++) {
    names[i] = (uint16_t)*(reply->mutable_env_get_database_names_reply()->mutable_names()->mutable_data() + i);
  }

  *count = i;
  return (0);
}

ups_status_t
RemoteEnvironment::do_get_parameters(ups_parameter_t *param)
{
  static char filename[1024]; // TODO not threadsafe!!
  ups_parameter_t *p = param;

  Protocol request(Protocol::ENV_GET_PARAMETERS_REQUEST);
  request.mutable_env_get_parameters_request()->set_env_handle(m_remote_handle);
  while (p && p->name != 0) {
    request.mutable_env_get_parameters_request()->add_names(p->name);
    p++;
  }

  ScopedPtr<Protocol> reply(perform_request(&request));

  assert(reply->has_env_get_parameters_reply());

  ups_status_t st = reply->env_get_parameters_reply().status();
  if (st)
    return (st);

  p = param;
  while (p && p->name) {
    switch (p->name) {
    case UPS_PARAM_CACHESIZE:
      assert(reply->env_get_parameters_reply().has_cache_size());
      p->value = reply->env_get_parameters_reply().cache_size();
      break;
    case UPS_PARAM_PAGESIZE:
      assert(reply->env_get_parameters_reply().has_page_size());
      p->value = reply->env_get_parameters_reply().page_size();
      break;
    case UPS_PARAM_MAX_DATABASES:
      assert(reply->env_get_parameters_reply().has_max_env_databases());
      p->value = reply->env_get_parameters_reply().max_env_databases();
      break;
    case UPS_PARAM_FLAGS:
      assert(reply->env_get_parameters_reply().has_flags());
      p->value = reply->env_get_parameters_reply().flags();
      break;
    case UPS_PARAM_FILEMODE:
      assert(reply->env_get_parameters_reply().has_filemode());
      p->value = reply->env_get_parameters_reply().filemode();
      break;
    case UPS_PARAM_FILENAME:
      if (reply->env_get_parameters_reply().has_filename()) {
        strncpy(filename, reply->env_get_parameters_reply().filename().c_str(),
              sizeof(filename) - 1);
        filename[sizeof(filename) - 1] = 0;
        p->value = (uint64_t)(&filename[0]);
      }
      break;
    default:
      ups_trace(("unknown parameter %d", (int)p->name));
      break;
    }
    p++;
  }
  return (0);
}

ups_status_t
RemoteEnvironment::do_flush(uint32_t flags)
{
  Protocol request(Protocol::ENV_FLUSH_REQUEST);
  request.mutable_env_flush_request()->set_flags(flags);
  request.mutable_env_flush_request()->set_env_handle(m_remote_handle);

  ScopedPtr<Protocol> reply(perform_request(&request));

  assert(reply->has_env_flush_reply());

  return (reply->env_flush_reply().status());
}

ups_status_t
RemoteEnvironment::do_create_db(Database **pdb, DbConfig &config,
                const ups_parameter_t *param)
{
  Protocol request(Protocol::ENV_CREATE_DB_REQUEST);
  request.mutable_env_create_db_request()->set_env_handle(m_remote_handle);
  request.mutable_env_create_db_request()->set_dbname(config.db_name);
  request.mutable_env_create_db_request()->set_flags(config.flags);

  const ups_parameter_t *p = param;
  if (p) {
    for (; p->name; p++) {
      if (p->name == UPS_PARAM_CUSTOM_COMPARE_NAME) {
        const char *zname = reinterpret_cast<const char *>(p->value);
        request.mutable_env_create_db_request()->set_compare_name(zname);
      }
      else {
        request.mutable_env_create_db_request()->add_param_names(p->name);
        request.mutable_env_create_db_request()->add_param_values(p->value);
      }
    }
  }

  ScopedPtr<Protocol> reply(perform_request(&request));

  assert(reply->has_env_create_db_reply());

  ups_status_t st = reply->env_create_db_reply().status();
  if (st)
    return (st);

  config.flags = reply->env_create_db_reply().db_flags();
  RemoteDatabase *rdb = new RemoteDatabase(this, config,
                            reply->env_create_db_reply().db_handle());

  *pdb = rdb;
  return (0);
}

ups_status_t
RemoteEnvironment::do_open_db(Database **pdb, DbConfig &config,
                const ups_parameter_t *param)
{
  Protocol request(Protocol::ENV_OPEN_DB_REQUEST);
  request.mutable_env_open_db_request()->set_env_handle(m_remote_handle);
  request.mutable_env_open_db_request()->set_dbname(config.db_name);
  request.mutable_env_open_db_request()->set_flags(config.flags);

  const ups_parameter_t *p = param;
  if (p) {
    for (; p->name; p++) {
      request.mutable_env_open_db_request()->add_param_names(p->name);
      request.mutable_env_open_db_request()->add_param_values(p->value);
    }
  }

  ScopedPtr<Protocol> reply(perform_request(&request));

  assert(reply->has_env_open_db_reply());

  ups_status_t st = reply->env_open_db_reply().status();
  if (st)
    return (st);

  config.flags = reply->env_open_db_reply().db_flags();
  RemoteDatabase *rdb = new RemoteDatabase(this, config,
                            reply->env_open_db_reply().db_handle());

  *pdb = rdb;
  return (0);
}

ups_status_t
RemoteEnvironment::do_rename_db( uint16_t oldname, uint16_t newname,
                uint32_t flags)
{
  Protocol request(Protocol::ENV_RENAME_REQUEST);
  request.mutable_env_rename_request()->set_env_handle(m_remote_handle);
  request.mutable_env_rename_request()->set_oldname(oldname);
  request.mutable_env_rename_request()->set_newname(newname);
  request.mutable_env_rename_request()->set_flags(flags);

  ScopedPtr<Protocol> reply(perform_request(&request));

  assert(reply->has_env_rename_reply());

  return (reply->env_rename_reply().status());
}

ups_status_t
RemoteEnvironment::do_erase_db(uint16_t name, uint32_t flags)
{
  Protocol request(Protocol::ENV_ERASE_DB_REQUEST);
  request.mutable_env_erase_db_request()->set_env_handle(m_remote_handle);
  request.mutable_env_erase_db_request()->set_name(name);
  request.mutable_env_erase_db_request()->set_flags(flags);

  ScopedPtr<Protocol> reply(perform_request(&request));

  assert(reply->has_env_erase_db_reply());

  return (reply->env_erase_db_reply().status());
}

Transaction *
RemoteEnvironment::do_txn_begin(const char *name, uint32_t flags)
{
  SerializedWrapper request;
  request.id = kTxnBeginRequest;
  request.txn_begin_request.env_handle = m_remote_handle;
  request.txn_begin_request.flags = flags;
  if (name) {
    request.txn_begin_request.name.value = (uint8_t *)name;
    request.txn_begin_request.name.size = strlen(name) + 1;
  }

  SerializedWrapper reply;
  perform_request(&request, &reply);
  assert(reply.id == kTxnBeginReply);

  ups_status_t st = reply.txn_begin_reply.status;
  if (st)
    throw Exception(st);

  Transaction *txn = new RemoteTransaction(this, name, flags,
                  reply.txn_begin_reply.txn_handle);
  m_txn_manager->begin(txn);
  return (txn);
}

ups_status_t
RemoteEnvironment::do_txn_commit(Transaction *txn, uint32_t flags)
{
  RemoteTransaction *rtxn = dynamic_cast<RemoteTransaction *>(txn);

  SerializedWrapper request;
  request.id = kTxnCommitRequest;
  request.txn_commit_request.txn_handle = rtxn->get_remote_handle();
  request.txn_commit_request.flags = flags;

  SerializedWrapper reply;
  perform_request(&request, &reply);
  assert(reply.id == kTxnCommitReply);

  ups_status_t st = reply.txn_commit_reply.status;
  if (st)
    return (st);

  return (m_txn_manager->commit(txn, flags));
}

ups_status_t
RemoteEnvironment::do_txn_abort(Transaction *txn, uint32_t flags)
{
  RemoteTransaction *rtxn = dynamic_cast<RemoteTransaction *>(txn);

  SerializedWrapper request;
  request.id = kTxnAbortRequest;
  request.txn_abort_request.txn_handle = rtxn->get_remote_handle();
  request.txn_abort_request.flags = flags;

  SerializedWrapper reply;
  perform_request(&request, &reply);
  assert(reply.id == kTxnAbortReply);
  ups_status_t st = reply.txn_abort_reply.status;
  if (st)
    return (st);

  return (m_txn_manager->abort(txn, flags));
}

ups_status_t
RemoteEnvironment::do_close(uint32_t flags)
{
  Protocol request(Protocol::DISCONNECT_REQUEST);
  request.mutable_disconnect_request()->set_env_handle(m_remote_handle);

  ScopedPtr<Protocol> reply(perform_request(&request));

  // ignore the reply

  m_socket.close();
  m_remote_handle = 0;
  return (0);
}

void
RemoteEnvironment::do_fill_metrics(ups_env_metrics_t *metrics) const
{
  throw Exception(UPS_NOT_IMPLEMENTED);
}

} // namespace upscaledb

#endif // UPS_ENABLE_REMOTE

