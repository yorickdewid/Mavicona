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
 * RecordList implementations for duplicate records
 *
 * Duplicate records are stored inline till a certain threshold limit
 * (m_duptable_threshold) is reached. In this case the duplicates are stored
 * in a separate blob (the DuplicateTable), and the previously occupied storage
 * in the node is reused for other records.
 *
 * Since records therefore have variable length, an UpfrontIndex is used
 * (see btree_keys_varlen.h).
 *
 * This file has two RecordList implementations:
 *
 *  - DuplicateRecordList: stores regular records as duplicates; records
 *          are stored as blobs if their size exceeds 8 bytes. Otherwise
 *          they are stored inline.
 *
 *  - DuplicateInlineRecordList: stores small fixed length records as
 *          duplicates
 */

#ifndef UPS_BTREE_RECORDS_DUPLICATE_H
#define UPS_BTREE_RECORDS_DUPLICATE_H

#include "0root/root.h"

#include <algorithm>
#include <iostream>
#include <vector>
#include <map>

// Always verify that a file of level N does not include headers > N!
#include "1globals/globals.h"
#include "1base/scoped_ptr.h"
#include "1base/dynamic_array.h"
#include "2page/page.h"
#include "3blob_manager/blob_manager.h"
#include "3btree/btree_node.h"
#include "3btree/btree_index.h"
#include "3btree/upfront_index.h"
#include "3btree/btree_records_base.h"
#include "4env/env_local.h"

#ifndef UPS_ROOT_H
#  error "root.h was not included"
#endif

namespace upscaledb {

namespace DefLayout {

// helper function which returns true if a record is inline
static bool is_record_inline(uint8_t flags) {
  return (flags != 0);
}

//
// A helper class for dealing with extended duplicate tables
//
//  Byte [0..3] - count
//       [4..7] - capacity
//       [8.. [ - the record list
//                  if m_inline_records:
//                      each record has n bytes record-data
//                  else
//                      each record has 1 byte flags, n bytes record-data
//
class DuplicateTable
{
  public:
    // Constructor; the flag |inline_records| indicates whether record
    // flags should be stored for each record. |record_size| is the
    // fixed length size of each record, or UPS_RECORD_SIZE_UNLIMITED
    DuplicateTable(LocalDatabase *db, bool inline_records, size_t record_size)
      : m_db(db), m_store_flags(!inline_records), m_record_size(record_size),
        m_inline_records(inline_records), m_table_id(0) {
    }

    // Allocates and fills the table; returns the new table id.
    // Can allocate empty tables (required for testing purposes).
    // The initial capacity of the table is twice the current
    // |record_count|.
    uint64_t create(Context *context, const uint8_t *data,
                    size_t record_count) {
      assert(m_table_id == 0);

      // This sets the initial capacity as described above
      size_t capacity = record_count * 2;
      m_table.resize(8 + capacity * record_width());
      if (likely(record_count > 0))
        m_table.overwrite(8, data, (m_inline_records
                                    ? m_record_size * record_count
                                    : 9 * record_count));

      set_record_count(record_count);
      set_record_capacity(record_count * 2);

      // Flush the table to disk, returns the blob-id of the table
      return (flush_duplicate_table(context));
    }

    // Reads the table from disk
    void open(Context *context, uint64_t table_id) {
      ups_record_t record = {0};
      m_db->lenv()->blob_manager()->read(context, table_id,
                      &record, UPS_FORCE_DEEP_COPY, &m_table);
      m_table_id = table_id;
    }

    // Returns the number of duplicates in that table
    int record_count() const {
      assert(m_table.size() > 4);
      return ((int) *(uint32_t *)m_table.data());
    }

    // Returns the record size of a duplicate
    uint32_t record_size(Context *context, int duplicate_index) {
      assert(duplicate_index < record_count());
      if (m_inline_records)
        return (m_record_size);
      assert(m_store_flags == true);

      uint8_t *precord_flags;
      uint8_t *p = record_data(duplicate_index, &precord_flags);
      uint8_t flags = *precord_flags;

      if (flags & BtreeRecord::kBlobSizeTiny)
        return (p[sizeof(uint64_t) - 1]);
      if (flags & BtreeRecord::kBlobSizeSmall)
        return (sizeof(uint64_t));
      if (flags & BtreeRecord::kBlobSizeEmpty)
        return (0);

      uint64_t blob_id = *(uint64_t *)p;
      return (m_db->lenv()->blob_manager()->blob_size(context, blob_id));
    }

    // Returns the full record and stores it in |record|. |flags| can
    // be 0 or |UPS_DIRECT_ACCESS|. These are the default
    // flags of ups_db_find et al.
    void record(Context *context, ByteArray *arena, ups_record_t *record,
                    uint32_t flags, int duplicate_index) {
      assert(duplicate_index < record_count());
      bool direct_access = (flags & UPS_DIRECT_ACCESS) != 0;

      uint8_t *precord_flags;
      uint8_t *p = record_data(duplicate_index, &precord_flags);
      uint8_t record_flags = precord_flags ? *precord_flags : 0;

      if (m_inline_records) {
        record->size = m_record_size;
        if (direct_access)
          record->data = p;
        else {
          if ((record->flags & UPS_RECORD_USER_ALLOC) == 0) {
            arena->resize(record->size);
            record->data = arena->data();
          }
          memcpy(record->data, p, m_record_size);
        }
        return;
      }

      assert(m_store_flags == true);

      if (record_flags & BtreeRecord::kBlobSizeEmpty) {
        record->data = 0;
        record->size = 0;
        return;
      }

      if (record_flags & BtreeRecord::kBlobSizeTiny) {
        record->size = p[sizeof(uint64_t) - 1];
        if (direct_access)
          record->data = &p[0];
        else {
          if ((record->flags & UPS_RECORD_USER_ALLOC) == 0) {
            arena->resize(record->size);
            record->data = arena->data();
          }
          memcpy(record->data, &p[0], record->size);
        }
        return;
      }

      if (record_flags & BtreeRecord::kBlobSizeSmall) {
        record->size = sizeof(uint64_t);
        if (direct_access)
          record->data = &p[0];
        else {
          if ((record->flags & UPS_RECORD_USER_ALLOC) == 0) {
            arena->resize(record->size);
            record->data = arena->data();
          }
          memcpy(record->data, &p[0], record->size);
        }
        return;
      }

      uint64_t blob_id = *(uint64_t *)p;

      // the record is stored as a blob
      LocalEnvironment *env = m_db->lenv();
      env->blob_manager()->read(context, blob_id, record, flags, arena);
    }

    // Updates the record of a key. Analog to the set_record() method
    // of the NodeLayout class. Returns the new table id and the
    // new duplicate index, if |new_duplicate_index| is not null.
    uint64_t set_record(Context *context, int duplicate_index,
                    ups_record_t *record, uint32_t flags,
                    uint32_t *new_duplicate_index) {
      BlobManager *blob_manager = m_db->lenv()->blob_manager();

      // the duplicate is overwritten
      if (flags & UPS_OVERWRITE) {
        uint8_t *record_flags = 0;
        uint8_t *p = record_data(duplicate_index, &record_flags);

        // the record is stored inline w/ fixed length?
        if (m_inline_records) {
          assert(record->size == m_record_size);
          memcpy(p, record->data, record->size);
          return (flush_duplicate_table(context));
        }
        // the existing record is a blob
        if (!is_record_inline(*record_flags)) {
          uint64_t ptr = *(uint64_t *)p;
          // overwrite the blob record
          if (record->size > sizeof(uint64_t)) {
            *(uint64_t *)p = blob_manager->overwrite(context, ptr,
                            record, flags);
            return (flush_duplicate_table(context));
          }
          // otherwise delete it and continue
          blob_manager->erase(context, ptr, 0);
        }
      }

      // If the key is not overwritten but inserted or appended: create a
      // "gap" in the table
      else {
        int count = record_count();

        // check for overflow
        if (unlikely(count == std::numeric_limits<int>::max())) {
          ups_log(("Duplicate table overflow"));
          throw Exception(UPS_LIMITS_REACHED);
        }

        // adjust flags
        if (flags & UPS_DUPLICATE_INSERT_BEFORE && duplicate_index == 0)
          flags |= UPS_DUPLICATE_INSERT_FIRST;
        else if (flags & UPS_DUPLICATE_INSERT_AFTER) {
          if (duplicate_index == count)
            flags |= UPS_DUPLICATE_INSERT_LAST;
          else {
            flags |= UPS_DUPLICATE_INSERT_BEFORE;
            duplicate_index++;
          }
        }

        // resize the table, if necessary
        if (unlikely(count == record_capacity()))
          grow_duplicate_table();

        // handle overwrites or inserts/appends
        if (flags & UPS_DUPLICATE_INSERT_FIRST) {
          if (count) {
            uint8_t *ptr = raw_record_data(0);
            memmove(ptr + record_width(), ptr,
                            count * record_width());
          }
          duplicate_index = 0;
        }
        else if (flags & UPS_DUPLICATE_INSERT_BEFORE) {
          uint8_t *ptr = raw_record_data(duplicate_index);
          memmove(ptr + record_width(), ptr,
                      (count - duplicate_index) * record_width());
        }
        else // UPS_DUPLICATE_INSERT_LAST
          duplicate_index = count;

        set_record_count(count + 1);
      }

      uint8_t *record_flags = 0;
      uint8_t *p = record_data(duplicate_index, &record_flags);

      // store record inline?
      if (m_inline_records) {
          assert(m_record_size == record->size);
          if (m_record_size > 0)
            memcpy(p, record->data, record->size);
      }
      else if (record->size == 0) {
        memcpy(p, "\0\0\0\0\0\0\0\0", 8);
        *record_flags = BtreeRecord::kBlobSizeEmpty;
      }
      else if (record->size < sizeof(uint64_t)) {
        p[sizeof(uint64_t) - 1] = (uint8_t)record->size;
        memcpy(&p[0], record->data, record->size);
        *record_flags = BtreeRecord::kBlobSizeTiny;
      }
      else if (record->size == sizeof(uint64_t)) {
        memcpy(&p[0], record->data, record->size);
        *record_flags = BtreeRecord::kBlobSizeSmall;
      }
      else {
        *record_flags = 0;
        uint64_t blob_id = blob_manager->allocate(context, record, flags);
        memcpy(p, &blob_id, sizeof(blob_id));
      }

      if (new_duplicate_index)
        *new_duplicate_index = duplicate_index;

      // write the duplicate table to disk and return the table-id
      return (flush_duplicate_table(context));
    }

    // Deletes a record from the table; also adjusts the count. If
    // |all_duplicates| is true or if the last element of the table is
    // deleted then the table itself will also be deleted. Returns 0
    // if this is the case, otherwise returns the table id.
    uint64_t erase_record(Context *context, int duplicate_index,
                    bool all_duplicates) {
      int count = record_count();

      if (count == 1 && duplicate_index == 0)
        all_duplicates = true;

      if (all_duplicates) {
        if (m_store_flags && !m_inline_records) {
          for (int i = 0; i < count; i++) {
            uint8_t *record_flags;
            uint8_t *p = record_data(i, &record_flags);
            if (is_record_inline(*record_flags))
              continue;
            if (*(uint64_t *)p != 0) {
              m_db->lenv()->blob_manager()->erase(context, *(uint64_t *)p);
              *(uint64_t *)p = 0;
            }
          }
        }
        if (m_table_id != 0)
          m_db->lenv()->blob_manager()->erase(context, m_table_id);
        set_record_count(0);
        m_table_id = 0;
        return (0);
      }

      assert(count > 0 && duplicate_index < count);

      uint8_t *record_flags;
      uint8_t *lhs = record_data(duplicate_index, &record_flags);
      if (record_flags != 0 && *record_flags == 0 && !m_inline_records) {
        m_db->lenv()->blob_manager()->erase(context, *(uint64_t *)lhs);
        *(uint64_t *)lhs = 0;
      }

      if (duplicate_index < count - 1) {
        lhs = raw_record_data(duplicate_index);
        uint8_t *rhs = lhs + record_width();
        memmove(lhs, rhs, record_width()
                        * (count - duplicate_index - 1));
      }

      // adjust the counter
      set_record_count(count - 1);

      // write the duplicate table to disk and return the table-id
      return (flush_duplicate_table(context));
    }

    // Returns the maximum capacity of elements in a duplicate table
    // This method could be private, but it's required by the unittests
    int record_capacity() const {
      assert(m_table.size() >= 8);
      return ((int) *(uint32_t *)(m_table.data() + 4));
    }

  private:
    // Doubles the capacity of the ByteArray which backs the table
    void grow_duplicate_table() {
      int capacity = record_capacity();
      if (capacity == 0)
        capacity = 8;
      m_table.resize(8 + (capacity * 2) * record_width());
      set_record_capacity(capacity * 2);
    }

    // Writes the modified duplicate table to disk; returns the new
    // table-id
    uint64_t flush_duplicate_table(Context *context) {
      ups_record_t record = {0};
      record.data = m_table.data();
      record.size = m_table.size();
      if (!m_table_id)
        m_table_id = m_db->lenv()->blob_manager()->allocate(
                        context, &record, 0);
      else
        m_table_id = m_db->lenv()->blob_manager()->overwrite(
                        context, m_table_id, &record, 0);
      return (m_table_id);
    }

    // Returns the size of a record structure in the ByteArray
    size_t record_width() const {
      if (m_inline_records)
        return (m_record_size);
      assert(m_store_flags == true);
      return (sizeof(uint64_t) + 1);
    }

    // Returns a pointer to the record data (including flags)
    uint8_t *raw_record_data(int duplicate_index) {
      if (m_inline_records)
        return (m_table.data() + 8 + m_record_size * duplicate_index);
      else
        return (m_table.data() + 8 + 9 * duplicate_index);
    }

    // Returns a pointer to the record data, and the flags
    uint8_t *record_data(int duplicate_index, uint8_t **pflags = 0) {
      uint8_t *p = raw_record_data(duplicate_index);
      if (m_store_flags) {
        if (pflags)
          *pflags = p++;
        else
          p++;
      }
      else if (pflags)
        *pflags = 0;
      return (p);
    }

    // Sets the number of used elements in a duplicate table
    void set_record_count(int record_count) {
      *(uint32_t *)m_table.data() = (uint32_t)record_count;
    }

    // Sets the maximum capacity of elements in a duplicate table
    void set_record_capacity(int capacity) {
      assert(m_table.size() >= 8);
      *(uint32_t *)(m_table.data() + 4) = (uint32_t)capacity;
    }

    // The database
    LocalDatabase *m_db;

    // Whether to store flags per record or not (true unless records
    // have constant length)
    bool m_store_flags;

    // The constant length record size, or UPS_RECORD_SIZE_UNLIMITED
    size_t m_record_size;

    // Stores the actual data of the table
    ByteArray m_table;

    // True if records are inline
    bool m_inline_records;

    // The blob id for persisting the table
    uint64_t m_table_id;
};

//
// Common functions for duplicate record lists
//
class DuplicateRecordList : public BaseRecordList
{
  protected:
    // for caching external duplicate tables
    typedef std::map<uint64_t, DuplicateTable *> DuplicateTableCache;

  public:
    enum {
      // A flag whether this RecordList has sequential data
      kHasSequentialData = 0
    };

    // Constructor
    DuplicateRecordList(LocalDatabase *db, PBtreeNode *node,
                    bool store_flags, size_t record_size)
      : m_db(db), m_node(node), m_index(db), m_data(0),
        m_store_flags(store_flags), m_record_size(record_size) {
      size_t page_size = db->lenv()->config().page_size_bytes;
      if (Globals::ms_duplicate_threshold)
        m_duptable_threshold = Globals::ms_duplicate_threshold;
      else {
        if (page_size == 1024)
          m_duptable_threshold = 8;
        else if (page_size <= 1024 * 8)
          m_duptable_threshold = 12;
        else if (page_size <= 1024 * 16)
          m_duptable_threshold = 20;
        else if (page_size <= 1024 * 32)
          m_duptable_threshold = 32;
        else {
          // 0x7f/127 is the maximum that we can store in the record
          // counter (7 bits), but we won't exploit this fully
          m_duptable_threshold = 64;
        }
      }

      // UpfrontIndex's chunk_size is just 1 byte (max 255); make sure that
      // the duplicate list fits into a single chunk!
      size_t rec_size = m_record_size;
      if (rec_size == UPS_RECORD_SIZE_UNLIMITED)
        rec_size = 9;
      if (m_duptable_threshold * rec_size > 250)
        m_duptable_threshold = 250 / rec_size;
    }

    // Destructor - clears the cache
    ~DuplicateRecordList() {
      if (m_duptable_cache) {
        for (DuplicateTableCache::iterator it = m_duptable_cache->begin();
                        it != m_duptable_cache->end(); it++)
          delete it->second;
      }
    }

    // Opens an existing RecordList
    void open(uint8_t *ptr, size_t range_size, size_t node_count) {
      m_data = ptr;
      m_index.open(m_data, range_size);
      m_range_size = range_size;
    }

    // Returns a duplicate table; uses a cache to speed up access
    DuplicateTable *duplicate_table(Context *context, uint64_t table_id) {
      if (!m_duptable_cache)
        m_duptable_cache.reset(new DuplicateTableCache());
      else {
        DuplicateTableCache::iterator it = m_duptable_cache->find(table_id);
        if (it != m_duptable_cache->end())
          return (it->second);
      }

      DuplicateTable *dt = new DuplicateTable(m_db, !m_store_flags,
                                m_record_size);
      dt->open(context, table_id);
      (*m_duptable_cache)[table_id] = dt;
      return (dt);
    }

    // Updates the DupTableCache and changes the table id of a DuplicateTable.
    // Called whenever a DuplicateTable's size increases, and the new blob-id
    // differs from the old one.
    void update_duplicate_table_id(DuplicateTable *dt,
                    uint64_t old_table_id, uint64_t new_table_id) {
      m_duptable_cache->erase(old_table_id);
      (*m_duptable_cache)[new_table_id] = dt;
    }

    // Erases a slot. Only updates the UpfrontIndex; does NOT delete the
    // record blobs!
    void erase(Context *context, size_t node_count, int slot) {
      m_index.erase(node_count, slot);
    }

    // Inserts a slot for one additional record
    void insert(Context *context, size_t node_count, int slot) {
      m_index.insert(node_count, slot);
    }

    // Copies |count| items from this[sstart] to dest[dstart]
    void copy_to(int sstart, size_t node_count,
                    DuplicateRecordList &dest, size_t other_node_count,
                    int dstart) {
      // make sure that the other node has sufficient capacity in its
      // UpfrontIndex
      dest.m_index.change_range_size(other_node_count, 0, 0,
                      m_index.get_capacity());

      uint32_t doffset;
      for (size_t i = 0; i < node_count - sstart; i++) {
        size_t size = m_index.get_chunk_size(sstart + i);

        dest.m_index.insert(other_node_count + i, dstart + i);
        // destination offset
        doffset = dest.m_index.allocate_space(other_node_count + i + 1,
                dstart + i, size);
        doffset = dest.m_index.get_absolute_offset(doffset);
        // source offset
        uint32_t soffset = m_index.get_chunk_offset(sstart + i);
        soffset = m_index.get_absolute_offset(soffset);
        // copy the data
        memcpy(&dest.m_data[doffset], &m_data[soffset], size);
      }

      // After copying, the caller will reduce the node count drastically.
      // Therefore invalidate the cached next_offset.
      m_index.invalidate_next_offset();
    }

    // Rearranges the list
    void vacuumize(size_t node_count, bool force) {
      if (force)
        m_index.increase_vacuumize_counter(100);
      m_index.maybe_vacuumize(node_count);
    }

  protected:
    // The database
    LocalDatabase *m_db;

    // The current node
    PBtreeNode *m_node;

    // The index which manages variable length chunks
    UpfrontIndex m_index;

    // The actual data of the node
    uint8_t *m_data;

    // Whether record flags are required
    bool m_store_flags;

    // The constant record size, or UPS_RECORD_SIZE_UNLIMITED
    size_t m_record_size;

    // The duplicate threshold
    size_t m_duptable_threshold;

    // A cache for duplicate tables
    ScopedPtr<DuplicateTableCache> m_duptable_cache;
};

//
// RecordList for records with fixed length, with duplicates. It uses
// an UpfrontIndex to manage the variable length chunks.
//
// If a key has duplicates, then all duplicates are stored sequentially.
// If that duplicate list exceeds a certain threshold then they are moved
// to a DuplicateTable, which is stored as a blob.
//
//   Format for each slot:
//
//       1 byte meta data
//              bit 1 - 7: duplicate counter, if kExtendedDuplicates == 0
//              bit 8: kExtendedDuplicates
//       if kExtendedDuplicates == 0:
//              <counter> * <length> bytes 
//                  <length> byte data (always inline)
//       if kExtendedDuplicates == 1:
//              8 byte: record id of the extended duplicate table
//
class DuplicateInlineRecordList : public DuplicateRecordList
{
  public:
    // Constructor
    DuplicateInlineRecordList(LocalDatabase *db, PBtreeNode *node)
      : DuplicateRecordList(db, node, false, db->config().record_size),
        m_record_size(db->config().record_size) {
    }

    // Creates a new RecordList starting at |data|
    void create(uint8_t *data, size_t range_size) {
      m_data = data;
      m_index.create(m_data, range_size, range_size / full_record_size());
      m_range_size = range_size;
    }

    // Calculates the required size for a range with the specified |capacity|
    size_t required_range_size(size_t node_count) const {
      return (m_index.get_required_range_size(node_count));
    }

    // Returns the actual record size including overhead
    size_t full_record_size() const {
      return (1 + m_record_size + m_index.get_full_index_size());
    }

    // Returns the number of duplicates for a slot
    int record_count(Context *context, int slot) {
      uint32_t offset = m_index.get_absolute_chunk_offset(slot);
      if (m_data[offset] & BtreeRecord::kExtendedDuplicates) {
        DuplicateTable *dt = duplicate_table(context, record_id(slot));
        return ((int)dt->record_count());
      }
      
      return (m_data[offset] & 0x7f);
    }

    // Returns the size of a record; the size is always constant
    uint64_t record_size(Context *context, int slot,
                    int duplicate_index = 0) const {
      return (m_record_size);
    }

    // Returns the full record and stores it in |dest|
    void record(Context *context, int slot, ByteArray *arena,
                    ups_record_t *record, uint32_t flags,
                    int duplicate_index) {
      // forward to duplicate table?
      uint32_t offset = m_index.get_absolute_chunk_offset(slot);
      if (unlikely(m_data[offset] & BtreeRecord::kExtendedDuplicates)) {
        DuplicateTable *dt = duplicate_table(context, record_id(slot));
        dt->record(context, arena, record, flags, duplicate_index);
        return;
      }

      assert(duplicate_index < (int)inline_record_count(slot));
      bool direct_access = (flags & UPS_DIRECT_ACCESS) != 0;

      // the record is always stored inline
      const uint8_t *ptr = record_data(slot, duplicate_index);
      record->size = m_record_size;
      if (direct_access)
        record->data = (void *)ptr;
      else {
        if ((record->flags & UPS_RECORD_USER_ALLOC) == 0) {
          arena->resize(record->size);
          record->data = arena->data();
        }
        memcpy(record->data, ptr, m_record_size);
      }
    }

    // Adds or overwrites a record
    void set_record(Context *context, int slot, int duplicate_index,
                ups_record_t *record, uint32_t flags,
                uint32_t *new_duplicate_index = 0) {
      uint32_t chunk_offset = m_index.get_absolute_chunk_offset(slot);
      uint32_t current_size = m_index.get_chunk_size(slot);

      assert(m_record_size == record->size);

      // if the slot was not yet allocated: allocate new space, initialize
      // it and then overwrite the record
      if (current_size == 0) {
        duplicate_index = 0;
        flags |= UPS_OVERWRITE;
        chunk_offset = m_index.allocate_space(m_node->length(), slot,
                                    1 + m_record_size);
        chunk_offset = m_index.get_absolute_offset(chunk_offset);
        // clear the flags
        m_data[chunk_offset] = 0;

        set_inline_record_count(slot, 1);
      }

      // if there's no duplicate table, but we're not able to add another
      // duplicate because of size constraints, then offload all
      // existing duplicates to an external DuplicateTable
      uint32_t record_count = inline_record_count(slot);
      size_t required_size = 1 + (record_count + 1) * m_record_size;

      if (!(m_data[chunk_offset] & BtreeRecord::kExtendedDuplicates)
             && !(flags & UPS_OVERWRITE)) {
        bool force_duptable = record_count >= m_duptable_threshold;
        if (!force_duptable
              && !m_index.can_allocate_space(m_node->length(),
                            required_size))
          force_duptable = true;

        // update chunk_offset - it might have been modified if
        // m_index.can_allocate_space triggered a vacuumize() operation
        chunk_offset = m_index.get_absolute_chunk_offset(slot);

        // already too many duplicates, or the record does not fit? then
        // allocate an overflow duplicate list and move all duplicates to
        // this list
        if (force_duptable) {
          DuplicateTable *dt = new DuplicateTable(m_db, !m_store_flags,
                                        m_record_size);
          uint64_t table_id = dt->create(context, record_data(slot, 0),
                                        record_count);
          if (!m_duptable_cache)
            m_duptable_cache.reset(new DuplicateTableCache());
          (*m_duptable_cache)[table_id] = dt;

          // write the id of the duplicate table
          if (m_index.get_chunk_size(slot) < 8 + 1) {
            // do not erase the slot because it occupies so little space
            size_t node_count = m_node->length();
            // force a split in the caller if the duplicate table cannot
            // be inserted
            if (!m_index.can_allocate_space(node_count, 8 + 1))
              throw Exception(UPS_LIMITS_REACHED);
            m_index.allocate_space(node_count, slot, 8 + 1);
            chunk_offset = m_index.get_absolute_chunk_offset(slot);
          }

          m_data[chunk_offset] |= BtreeRecord::kExtendedDuplicates;
          set_record_id(slot, table_id);
          set_inline_record_count(slot, 0);

          m_index.set_chunk_size(slot, 8 + 1);
          m_index.increase_vacuumize_counter(m_index.get_chunk_size(slot) - 9);
          m_index.invalidate_next_offset();

          // fall through
        }
      }

      // forward to duplicate table?
      if (unlikely(m_data[chunk_offset] & BtreeRecord::kExtendedDuplicates)) {
        uint64_t table_id = record_id(slot);
        DuplicateTable *dt = duplicate_table(context, table_id);
        uint64_t new_table_id = dt->set_record(context, duplicate_index, record,
                        flags, new_duplicate_index);
        if (new_table_id != table_id) {
          update_duplicate_table_id(dt, table_id, new_table_id);
          set_record_id(slot, new_table_id);
        }
        return;
      }

      // the duplicate is overwritten
      if (flags & UPS_OVERWRITE) {
        // the record is always stored inline w/ fixed length
        uint8_t *p = (uint8_t *)record_data(slot, duplicate_index);
        memcpy(p, record->data, record->size);
        return;
      }

      // Allocate new space for the duplicate table, if required
      if (current_size < required_size) {
        uint8_t *oldp = &m_data[chunk_offset];
        uint32_t old_chunk_size = m_index.get_chunk_size(slot);
        uint32_t old_chunk_offset = m_index.get_chunk_offset(slot);
        uint32_t new_chunk_offset = m_index.allocate_space(m_node->length(),
                        slot, required_size);
        chunk_offset = m_index.get_absolute_offset(new_chunk_offset);
        if (current_size > 0 && old_chunk_offset != new_chunk_offset) {
          memmove(&m_data[chunk_offset], oldp, current_size);
          m_index.add_to_freelist(m_node->length(), old_chunk_offset,
                        old_chunk_size);
        }
      }

      // adjust flags
      if (flags & UPS_DUPLICATE_INSERT_BEFORE && duplicate_index == 0)
        flags |= UPS_DUPLICATE_INSERT_FIRST;
      else if (flags & UPS_DUPLICATE_INSERT_AFTER) {
        if (duplicate_index == (int)record_count)
          flags |= UPS_DUPLICATE_INSERT_LAST;
        else {
          flags |= UPS_DUPLICATE_INSERT_BEFORE;
          duplicate_index++;
        }
      }

      // handle overwrites or inserts/appends
      if (flags & UPS_DUPLICATE_INSERT_FIRST) {
        if (record_count > 0) {
          uint8_t *ptr = record_data(slot, 0);
          memmove(record_data(slot, 1), ptr, record_count * m_record_size);
        }
        duplicate_index = 0;
      }
      else if (flags & UPS_DUPLICATE_INSERT_BEFORE) {
        memmove(record_data(slot, duplicate_index),
                    record_data(slot, duplicate_index + 1),
                    (record_count - duplicate_index) * m_record_size);
      }
      else // UPS_DUPLICATE_INSERT_LAST
        duplicate_index = record_count;

      set_inline_record_count(slot, record_count + 1);

      // store the new record inline
      if (m_record_size > 0)
        memcpy(record_data(slot, duplicate_index),
                        record->data, record->size);

      if (new_duplicate_index)
        *new_duplicate_index = duplicate_index;
    }

    // Erases a record's blob (does not remove the slot!)
    void erase_record(Context *context, int slot, int duplicate_index = 0,
                    bool all_duplicates = false) {
      uint32_t offset = m_index.get_absolute_chunk_offset(slot);

      // forward to external duplicate table?
      if (unlikely(m_data[offset] & BtreeRecord::kExtendedDuplicates)) {
        uint64_t table_id = record_id(slot);
        DuplicateTable *dt = duplicate_table(context, table_id);
        uint64_t new_table_id = dt->erase_record(context, duplicate_index,
                        all_duplicates);
        if (new_table_id == 0) {
          m_duptable_cache->erase(table_id);
          set_record_id(slot, 0);
          m_data[offset] &= ~BtreeRecord::kExtendedDuplicates;
          delete dt;
        }
        else if (new_table_id != table_id) {
          update_duplicate_table_id(dt, table_id, new_table_id);
          set_record_id(slot, new_table_id);
        }
        return;
      }

      // there's only one record left which is erased?
      size_t duplicate_count = inline_record_count(slot);
      if (duplicate_count == 1 && duplicate_index == 0)
        all_duplicates = true;

      // erase all duplicates?
      if (all_duplicates) {
        set_inline_record_count(slot, 0);
      }
      else {
        if (duplicate_index < (int)duplicate_count - 1)
          ::memmove(record_data(slot, duplicate_index),
                      record_data(slot, duplicate_index + 1), 
                      m_record_size * (duplicate_count - duplicate_index - 1));
        set_inline_record_count(slot, duplicate_count - 1);
      }
    }

    // Returns a 64bit record id from a record
    uint64_t record_id(int slot,
                    int duplicate_index = 0) const {
      return (*(uint64_t *)record_data(slot, duplicate_index));
    }

    // Sets a 64bit record id; used for internal nodes to store Page IDs
    // or for leaf nodes to store DuplicateTable IDs
    void set_record_id(int slot, uint64_t id) {
      assert(m_index.get_chunk_size(slot) >= sizeof(id));
      *(uint64_t *)record_data(slot, 0) = id;
    }

    // Checks the integrity of this node. Throws an exception if there is a
    // violation.
    void check_integrity(Context *context, size_t node_count,
                    bool quick = false) const {
      for (size_t i = 0; i < node_count; i++) {
        uint32_t offset = m_index.get_absolute_chunk_offset(i);
        if (m_data[offset] & BtreeRecord::kExtendedDuplicates) {
          assert((m_data[offset] & 0x7f) == 0);
        }
      }

      m_index.check_integrity(node_count);
    }

    // Change the capacity; the capacity will be reduced, growing is not
    // implemented. Which means that the data area must be copied; the offsets
    // do not have to be changed.
    void change_range_size(size_t node_count, uint8_t *new_data_ptr,
                size_t new_range_size, size_t capacity_hint) {
      // no capacity given? then try to find a good default one
      if (capacity_hint == 0) {
        capacity_hint = (new_range_size - m_index.get_next_offset(node_count)
                - full_record_size()) / m_index.get_full_index_size();
        if (capacity_hint <= node_count)
          capacity_hint = node_count + 1;
      }

      // if there's not enough space for the new capacity then try to reduce
      // the capacity
      if (m_index.get_next_offset(node_count) + full_record_size()
                      + capacity_hint * m_index.get_full_index_size()
                      + UpfrontIndex::kPayloadOffset
                > new_range_size)
        capacity_hint = node_count + 1;

      m_index.change_range_size(node_count, new_data_ptr, new_range_size,
                capacity_hint);
      m_data = new_data_ptr;
      m_range_size = new_range_size;
    }

    // Returns true if there's not enough space for another record
    bool requires_split(size_t node_count) {
      // if the record is extremely small then make sure there's some headroom;
      // this is required for DuplicateTable ids which are 64bit numbers
      size_t required = full_record_size();
      if (required < 10)
        required = 10;
      return (m_index.requires_split(node_count, required));
    }

    // Iterates all records, calls the |visitor| on each
    ScanResult scan(ByteArray *arena, size_t node_count, uint32_t start) {
      assert(!"shouldn't be here");
      throw Exception(UPS_INTERNAL_ERROR);
    }

    // Fills the btree_metrics structure
    void fill_metrics(btree_metrics_t *metrics, size_t node_count) {
      BaseRecordList::fill_metrics(metrics, node_count);
      BtreeStatistics::update_min_max_avg(&metrics->recordlist_index,
                      m_index.get_capacity() * m_index.get_full_index_size());
      BtreeStatistics::update_min_max_avg(&metrics->recordlist_unused,
                          m_range_size - required_range_size(node_count));
    }

    // Prints a slot to |out| (for debugging)
    void print(Context *context, int slot, std::stringstream &out) {
      out << "(" << record_count(context, slot) << " records)";
    }

  private:
    // Returns the number of records that are stored inline
    uint32_t inline_record_count(int slot) {
      uint32_t offset = m_index.get_absolute_chunk_offset(slot);
      return (m_data[offset] & 0x7f);
    }

    // Sets the number of records that are stored inline
    void set_inline_record_count(int slot, size_t count) {
      assert(count <= 0x7f);
      uint32_t offset = m_index.get_absolute_chunk_offset(slot);
      m_data[offset] &= BtreeRecord::kExtendedDuplicates;
      m_data[offset] |= count;
    }

    // Returns a pointer to the record data
    uint8_t *record_data(int slot, int duplicate_index = 0) {
      uint32_t offset = m_index.get_absolute_chunk_offset(slot);
      return (&m_data[offset + 1 + m_record_size * duplicate_index]);
    }

    // Returns a pointer to the record data (const flavour)
    const uint8_t *record_data(int slot,
                        int duplicate_index = 0) const {
      uint32_t offset = m_index.get_absolute_chunk_offset(slot);
      return (&m_data[offset + 1 + m_record_size * duplicate_index]);
    }

    // The constant length record size
    size_t m_record_size;
};

//
// RecordList for default records (8 bytes; either inline or a record id),
// with duplicates
//
//   Format for each slot:
//
//       1 byte meta data
//              bit 1 - 7: duplicate counter, if kExtendedDuplicates == 0
//              bit 8: kExtendedDuplicates
//       if kExtendedDuplicates == 0:
//              <counter> * 9 bytes 
//                  1 byte flags (RecordFlag::*)
//                  8 byte data (either inline or record-id)
//       if kExtendedDuplicates == 1:
//              8 byte: record id of the extended duplicate table
//
class DuplicateDefaultRecordList : public DuplicateRecordList
{
  public:
    // Constructor
    DuplicateDefaultRecordList(LocalDatabase *db, PBtreeNode *node)
      : DuplicateRecordList(db, node, true, UPS_RECORD_SIZE_UNLIMITED) {
    }

    // Creates a new RecordList starting at |data|
    void create(uint8_t *data, size_t range_size) {
      m_data = data;
      m_index.create(m_data, range_size, range_size / full_record_size());
    }

    // Calculates the required size for a range with the specified |capacity|
    size_t required_range_size(size_t node_count) const {
      return (m_index.get_required_range_size(node_count));
    }

    // Returns the actual key record including overhead
    size_t full_record_size() const {
      return (1 + 1 + 8 + m_index.get_full_index_size());
    }

    // Returns the number of duplicates
    int record_count(Context *context, int slot) {
      uint32_t offset = m_index.get_absolute_chunk_offset(slot);
      if (unlikely(m_data[offset] & BtreeRecord::kExtendedDuplicates)) {
        DuplicateTable *dt = duplicate_table(context, record_id(slot));
        return ((int) dt->record_count());
      }
      
      return (m_data[offset] & 0x7f);
    }

    // Returns the size of a record
    uint64_t record_size(Context *context, int slot,
                    int duplicate_index = 0) {
      uint32_t offset = m_index.get_absolute_chunk_offset(slot);
      if (unlikely(m_data[offset] & BtreeRecord::kExtendedDuplicates)) {
        DuplicateTable *dt = duplicate_table(context, record_id(slot));
        return (dt->record_size(context, duplicate_index));
      }
      
      uint8_t *p = &m_data[offset + 1 + 9 * duplicate_index];
      uint8_t flags = *(p++);
      if (flags & BtreeRecord::kBlobSizeTiny)
        return (p[sizeof(uint64_t) - 1]);
      if (flags & BtreeRecord::kBlobSizeSmall)
        return (sizeof(uint64_t));
      if (flags & BtreeRecord::kBlobSizeEmpty)
        return (0);

      LocalEnvironment *env = m_db->lenv();
      return (env->blob_manager()->blob_size(context, *(uint64_t *)p));
    }

    // Returns the full record and stores it in |dest|; memory must be
    // allocated by the caller
    void record(Context *context, int slot, ByteArray *arena,
                    ups_record_t *record, uint32_t flags, int duplicate_index) {
      // forward to duplicate table?
      uint32_t offset = m_index.get_absolute_chunk_offset(slot);
      if (unlikely(m_data[offset] & BtreeRecord::kExtendedDuplicates)) {
        DuplicateTable *dt = duplicate_table(context, record_id(slot));
        dt->record(context, arena, record, flags, duplicate_index);
        return;
      }

      assert(duplicate_index < (int)inline_record_count(slot));
      bool direct_access = (flags & UPS_DIRECT_ACCESS) != 0;

      uint8_t *p = &m_data[offset + 1 + 9 * duplicate_index];
      uint8_t record_flags = *(p++);

      if (record_flags & BtreeRecord::kBlobSizeEmpty) {
        record->data = 0;
        record->size = 0;
        return;
      }

      if (record_flags & BtreeRecord::kBlobSizeTiny) {
        record->size = p[sizeof(uint64_t) - 1];
        if (direct_access)
          record->data = &p[0];
        else {
          if ((record->flags & UPS_RECORD_USER_ALLOC) == 0) {
            arena->resize(record->size);
            record->data = arena->data();
          }
          memcpy(record->data, &p[0], record->size);
        }
        return;
      }

      if (record_flags & BtreeRecord::kBlobSizeSmall) {
        record->size = sizeof(uint64_t);
        if (direct_access)
          record->data = &p[0];
        else {
          if ((record->flags & UPS_RECORD_USER_ALLOC) == 0) {
            arena->resize(record->size);
            record->data = arena->data();
          }
          memcpy(record->data, &p[0], record->size);
        }
        return;
      }

      uint64_t blob_id = *(uint64_t *)p;

      // the record is stored as a blob
      LocalEnvironment *env = m_db->lenv();
      env->blob_manager()->read(context, blob_id, record, flags, arena);
    }

    // Updates the record of a key
    void set_record(Context *context, int slot, int duplicate_index,
                ups_record_t *record, uint32_t flags,
                uint32_t *new_duplicate_index = 0) {
      uint32_t chunk_offset = m_index.get_absolute_chunk_offset(slot);
      uint32_t current_size = m_index.get_chunk_size(slot);

      // if the slot was not yet allocated: allocate new space, initialize
      // it and then overwrite the record
      if (current_size == 0) {
        duplicate_index = 0;
        flags |= UPS_OVERWRITE;
        chunk_offset = m_index.allocate_space(m_node->length(), slot, 1 + 9);
        chunk_offset = m_index.get_absolute_offset(chunk_offset);
        // clear the record flags
        m_data[chunk_offset] = 0;
        m_data[chunk_offset + 1] = BtreeRecord::kBlobSizeEmpty;

        set_inline_record_count(slot, 1);
      }

      // if there's no duplicate table, but we're not able to add another
      // duplicate then offload all existing duplicates to a table
      uint32_t record_count = inline_record_count(slot);
      size_t required_size = 1 + (record_count + 1) * 9;

      if (!(m_data[chunk_offset] & BtreeRecord::kExtendedDuplicates)
             && !(flags & UPS_OVERWRITE)) {
        bool force_duptable = record_count >= m_duptable_threshold;
        if (!force_duptable
              && !m_index.can_allocate_space(m_node->length(),
                            required_size))
          force_duptable = true;
      
        // update chunk_offset - it might have been modified if
        // m_index.can_allocate_space triggered a vacuumize() operation
        chunk_offset = m_index.get_absolute_chunk_offset(slot);

        // already too many duplicates, or the record does not fit? then
        // allocate an overflow duplicate list and move all duplicates to
        // this list
        if (force_duptable) {
          DuplicateTable *dt = new DuplicateTable(m_db, !m_store_flags,
                                        UPS_RECORD_SIZE_UNLIMITED);
          uint64_t table_id = dt->create(context, record_data(slot, 0),
                                    record_count);
          if (!m_duptable_cache)
            m_duptable_cache.reset(new DuplicateTableCache());
          (*m_duptable_cache)[table_id] = dt;

          // write the id of the duplicate table
          if (m_index.get_chunk_size(slot) < 8 + 1) {
            // do not erase the slot because it obviously occupies so
            // little space
            m_index.allocate_space(m_node->length(), slot, 8 + 1);
            chunk_offset = m_index.get_absolute_chunk_offset(slot);
          }

          m_data[chunk_offset] |= BtreeRecord::kExtendedDuplicates;
          set_record_id(slot, table_id);
          set_inline_record_count(slot, 0);

          m_index.set_chunk_size(slot, 10);
          m_index.increase_vacuumize_counter(m_index.get_chunk_size(slot) - 10);
          m_index.invalidate_next_offset();

          // fall through
        }
      }

      // forward to duplicate table?
      if (unlikely(m_data[chunk_offset] & BtreeRecord::kExtendedDuplicates)) {
        uint64_t table_id = record_id(slot);
        DuplicateTable *dt = duplicate_table(context, table_id);
        uint64_t new_table_id = dt->set_record(context, duplicate_index, record,
                        flags, new_duplicate_index);
        if (new_table_id != table_id) {
          update_duplicate_table_id(dt, table_id, new_table_id);
          set_record_id(slot, new_table_id);
        }
        return;
      }

      uint64_t overwrite_blob_id = 0;
      uint8_t *record_flags = 0;
      uint8_t *p = 0;

      // the (inline) duplicate is overwritten
      if (flags & UPS_OVERWRITE) {
        record_flags = &m_data[chunk_offset + 1 + 9 * duplicate_index];
        p = record_flags + 1;

        // If a blob is overwritten with an inline record then the old blob
        // has to be deleted
        if (*record_flags == 0) {
          if (record->size <= 8) {
            uint64_t blob_id = *(uint64_t *)p;
            if (blob_id)
              m_db->lenv()->blob_manager()->erase(context, blob_id);
          }
          else
            overwrite_blob_id = *(uint64_t *)p;
          // fall through
        }
        // then jump to the code which performs the actual insertion
        goto write_record;
      }

      // Allocate new space for the duplicate table, if required
      if (current_size < required_size) {
        uint8_t *oldp = &m_data[chunk_offset];
        uint32_t old_chunk_size = m_index.get_chunk_size(slot);
        uint32_t old_chunk_offset = m_index.get_chunk_offset(slot);
        uint32_t new_chunk_offset = m_index.allocate_space(m_node->length(),
                        slot, required_size);
        chunk_offset = m_index.get_absolute_offset(new_chunk_offset);
        if (current_size > 0)
          memmove(&m_data[chunk_offset], oldp, current_size);
        if (old_chunk_offset != new_chunk_offset)
          m_index.add_to_freelist(m_node->length(), old_chunk_offset,
                          old_chunk_size);
      }

      // adjust flags
      if (flags & UPS_DUPLICATE_INSERT_BEFORE && duplicate_index == 0)
        flags |= UPS_DUPLICATE_INSERT_FIRST;
      else if (flags & UPS_DUPLICATE_INSERT_AFTER) {
        if (duplicate_index == (int)record_count)
          flags |= UPS_DUPLICATE_INSERT_LAST;
        else {
          flags |= UPS_DUPLICATE_INSERT_BEFORE;
          duplicate_index++;
        }
      }

      // handle overwrites or inserts/appends
      if (flags & UPS_DUPLICATE_INSERT_FIRST) {
        if (record_count > 0) {
          uint8_t *ptr = &m_data[chunk_offset + 1];
          memmove(&m_data[chunk_offset + 1 + 9], ptr, record_count * 9);
        }
        duplicate_index = 0;
      }
      else if (flags & UPS_DUPLICATE_INSERT_BEFORE) {
        memmove(&m_data[chunk_offset + 1 + 9 * (duplicate_index + 1)],
                    &m_data[chunk_offset + 1 + 9 * duplicate_index],
                    (record_count - duplicate_index) * 9);
      }
      else // UPS_DUPLICATE_INSERT_LAST
        duplicate_index = record_count;

      set_inline_record_count(slot, record_count + 1);

      record_flags = &m_data[chunk_offset + 1 + 9 * duplicate_index];
      p = record_flags + 1;

write_record:
      if (record->size == 0) {
        memcpy(p, "\0\0\0\0\0\0\0\0", 8);
        *record_flags = BtreeRecord::kBlobSizeEmpty;
      }
      else if (record->size < sizeof(uint64_t)) {
        p[sizeof(uint64_t) - 1] = (uint8_t)record->size;
        memcpy(&p[0], record->data, record->size);
        *record_flags = BtreeRecord::kBlobSizeTiny;
      }
      else if (record->size == sizeof(uint64_t)) {
        memcpy(&p[0], record->data, record->size);
        *record_flags = BtreeRecord::kBlobSizeSmall;
      }
      else {
        LocalEnvironment *env = m_db->lenv();
        *record_flags = 0;
        uint64_t blob_id;
        if (overwrite_blob_id)
          blob_id = env->blob_manager()->overwrite(context,
                          overwrite_blob_id, record, flags);
        else
          blob_id = env->blob_manager()->allocate(context, record, flags);
        memcpy(p, &blob_id, sizeof(blob_id));
      }

      if (new_duplicate_index)
        *new_duplicate_index = duplicate_index;
    }

    // Erases a record
    void erase_record(Context *context, int slot, int duplicate_index = 0,
                    bool all_duplicates = false) {
      uint32_t offset = m_index.get_absolute_chunk_offset(slot);

      // forward to external duplicate table?
      if (unlikely(m_data[offset] & BtreeRecord::kExtendedDuplicates)) {
        uint64_t table_id = record_id(slot);
        DuplicateTable *dt = duplicate_table(context, table_id);
        uint64_t new_table_id = dt->erase_record(context, duplicate_index,
                        all_duplicates);
        if (new_table_id == 0) {
          m_duptable_cache->erase(table_id);
          set_record_id(slot, 0);
          m_data[offset] &= ~BtreeRecord::kExtendedDuplicates;
          delete dt;
        }
        else if (new_table_id != table_id) {
          update_duplicate_table_id(dt, table_id, new_table_id);
          set_record_id(slot, new_table_id);
        }
        return;
      }

      // erase the last duplicate?
      uint32_t count = inline_record_count(slot);
      if (count == 1 && duplicate_index == 0)
        all_duplicates = true;

      // adjust next_offset, if necessary. Note that next_offset() is
      // called with a node_count of zero, which is valid (it avoids a
      // recalculation in case there is no next_offset)
      m_index.maybe_invalidate_next_offset(m_index.get_chunk_offset(slot)
                      + m_index.get_chunk_size(slot));

      // erase all duplicates?
      if (all_duplicates) {
        for (uint32_t i = 0; i < count; i++) {
          uint8_t *p = &m_data[offset + 1 + 9 * i];
          if (!is_record_inline(*p)) {
            m_db->lenv()->blob_manager()->erase(context, *(uint64_t *)(p + 1));
            *(uint64_t *)(p + 1) = 0;
          }
        }
        set_inline_record_count(slot, 0);
        m_index.set_chunk_size(slot, 0);
      }
      else {
        uint8_t *p = &m_data[offset + 1 + 9 * duplicate_index];
        if (!is_record_inline(*p)) {
          m_db->lenv()->blob_manager()->erase(context, *(uint64_t *)(p + 1));
          *(uint64_t *)(p + 1) = 0;
        }
        if (duplicate_index < (int)count - 1)
          memmove(&m_data[offset + 1 + 9 * duplicate_index],
                  &m_data[offset + 1 + 9 * (duplicate_index + 1)],
                  9 * (count - duplicate_index - 1));
        set_inline_record_count(slot, count - 1);
      }
    }

    // Returns a record id
    uint64_t record_id(int slot, int duplicate_index = 0) const {
      return (*(uint64_t *)record_data(slot, duplicate_index));
    }

    // Sets a record id
    void set_record_id(int slot, uint64_t id) {
      *(uint64_t *)record_data(slot, 0) = id;
    }

    // Checks the integrity of this node. Throws an exception if there is a
    // violation.
    void check_integrity(Context *context, size_t node_count) const {
      for (size_t i = 0; i < node_count; i++) {
        uint32_t offset = m_index.get_absolute_chunk_offset(i);
        if (m_data[offset] & BtreeRecord::kExtendedDuplicates) {
          assert((m_data[offset] & 0x7f) == 0);
        }
      }

      m_index.check_integrity(node_count);
    }

    // Change the capacity; the capacity will be reduced, growing is not
    // implemented. Which means that the data area must be copied; the offsets
    // do not have to be changed.
    void change_range_size(size_t node_count, uint8_t *new_data_ptr,
                size_t new_range_size, size_t capacity_hint) {
      // no capacity given? then try to find a good default one
      if (capacity_hint == 0) {
        capacity_hint = (new_range_size - m_index.get_next_offset(node_count)
                - full_record_size()) / m_index.get_full_index_size();
        if (capacity_hint <= node_count)
          capacity_hint = node_count + 1;
      }

      // if there's not enough space for the new capacity then try to reduce
      // the capacity
      if (m_index.get_next_offset(node_count) + full_record_size()
                      + capacity_hint * m_index.get_full_index_size()
                      + UpfrontIndex::kPayloadOffset
                > new_range_size)
        capacity_hint = node_count + 1;

      m_index.change_range_size(node_count, new_data_ptr, new_range_size,
                capacity_hint);
      m_data = new_data_ptr;
      m_range_size = new_range_size;
    }

    // Returns true if there's not enough space for another record
    bool requires_split(size_t node_count) {
      // if the record is extremely small then make sure there's some headroom;
      // this is required for DuplicateTable ids which are 64bit numbers
      size_t required = full_record_size();
      if (required < 10)
        required = 10;
      return (m_index.requires_split(node_count, required));
    }

    // Iterates all records, calls the |visitor| on each
    ScanResult scan(ByteArray *arena, size_t node_count, uint32_t start) {
      assert(!"shouldn't be here");
      throw Exception(UPS_INTERNAL_ERROR);
    }

    // Fills the btree_metrics structure
    void fill_metrics(btree_metrics_t *metrics, size_t node_count) {
      BaseRecordList::fill_metrics(metrics, node_count);
      BtreeStatistics::update_min_max_avg(&metrics->recordlist_index,
                      m_index.get_capacity() * m_index.get_full_index_size());
      BtreeStatistics::update_min_max_avg(&metrics->recordlist_unused,
                          m_range_size - required_range_size(node_count));
    }

    // Prints a slot to |out| (for debugging)
    void print(Context *context, int slot, std::stringstream &out) {
      out << "(" << record_count(context, slot) << " records)";
    }

  private:
    // Returns the number of records that are stored inline
    uint32_t inline_record_count(int slot) {
      uint32_t offset = m_index.get_absolute_chunk_offset(slot);
      return (m_data[offset] & 0x7f);
    }

    // Sets the number of records that are stored inline
    void set_inline_record_count(int slot, size_t count) {
      assert(count <= 0x7f);
      uint32_t offset = m_index.get_absolute_chunk_offset(slot);
      m_data[offset] &= BtreeRecord::kExtendedDuplicates;
      m_data[offset] |= count;
    }

    // Returns a pointer to the record data (const flavour)
    uint8_t *record_data(int slot, int duplicate_index = 0) {
      uint32_t offset = m_index.get_absolute_chunk_offset(slot);
      return (&m_data[offset + 1 + 9 * duplicate_index]);
    }

    // Returns a pointer to the record data (const flavour)
    const uint8_t *record_data(int slot,
                        int duplicate_index = 0) const {
      uint32_t offset = m_index.get_absolute_chunk_offset(slot);
      return (&m_data[offset + 1 + 9 * duplicate_index]);
    }
};

} // namespace DefLayout

} // namespace upscaledb

#endif /* UPS_BTREE_RECORDS_DUPLICATE_H */
