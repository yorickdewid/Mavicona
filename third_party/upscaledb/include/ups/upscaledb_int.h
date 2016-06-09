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

/**
 * @file upscaledb_int.h
 * @brief Internal upscaledb functions.
 * @author Christoph Rupp, chris@crupp.de
 *
 * Please be aware that the interfaces in this file are mostly for internal
 * use. Unlike those in upscaledb.h they are not stable and can be changed
 * with every new version.
 *
 */

#ifndef UPS_UPSCALEDB_INT_H
#define UPS_UPSCALEDB_INT_H

#include <ups/upscaledb.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ups_extended_api upscaledb Extended API
 * @{
 */

/** get the (non-persisted) flags of a key */
#define ups_key_get_intflags(key)       (key)->_flags

/**
 * set the flags of a key
 *
 * Note that the ups_find/ups_cursor_find/ups_cursor_find_ex flags must
 * be defined such that those can peacefully co-exist with these; that's
 * why those public flags start at the value 0x1000 (4096).
 */
#define ups_key_set_intflags(key, f)    (key)->_flags=(f)

/**
 * Returns the kind of key match which produced this key as it was
 * returned by one of the @ref ups_db_find() and @ref ups_cursor_find().
 *
 * This routine assumes the key was passed back by one of the @ref ups_db_find
 * and @ref ups_cursor_find functions and not used by any other upscaledb
 * functions after that.
 *
 * As such, this function produces an answer akin to the 'sign' of the
 * specified key as it was returned by the find operation.
 *
 * @param key A valid key
 *
 * @return 1 (greater than) or -1 (less than) when the given key is an
 *    approximate result / zero (0) otherwise. Specifically:
 *    <ul>
 *    <li>+1 when the key is greater than the item searched for (key
 *        was a GT match)
 *    <li>-1 when the key is less than the item searched for (key was
 *        a LT match)
 *    <li>zero (0) otherwise (key was an EQ (EXACT) match)
 *    </ul>
 */
UPS_EXPORT int UPS_CALLCONV
ups_key_get_approximate_match_type(ups_key_t *key);

/**
 * Verifies the integrity of the Database
 *
 * This function is only interesting if you want to debug upscaledb.
 *
 * @param db A valid Database handle
 * @param flags Optional flags for the integrity check, combined with
 *      bitwise OR. Possible flags are:
 *    <ul>
 *     <li>@ref UPS_PRINT_GRAPH</li> Prints the Btree as a graph; stores
 *      the image as "graph.png" in the current working directory. It uses
 *      the "dot" tool from graphviz to generate the image.
 *      This functionality is only available in DEBUG builds!
 *    </ul>
 *
 * @return @ref UPS_SUCCESS upon success
 * @return @ref UPS_INTEGRITY_VIOLATED if the Database is broken
 */
UPS_EXPORT ups_status_t UPS_CALLCONV
ups_db_check_integrity(ups_db_t *db, uint32_t flags);

/** Flag for ups_db_check_integrity */
#define UPS_PRINT_GRAPH             1

/**
 * Set a user-provided context pointer
 *
 * This function sets a user-provided context pointer. This can be any
 * arbitrary pointer; it is stored in the Database handle and can be
 * retrieved with @a ups_get_context_data. It is mainly used by Wrappers
 * and language bindings.
 *
 * @param db A valid Database handle
 * @param data The pointer to the context data
 */
UPS_EXPORT void UPS_CALLCONV
ups_set_context_data(ups_db_t *db, void *data);

/**
 * Retrieves a user-provided context pointer
 *
 * This function retrieves a user-provided context pointer. This can be any
 * arbitrary pointer which was previously stored with @a ups_set_context_data.
 *
 * @param db A valid Database handle
 * @param dont_lock Whether the Environment mutex should be locked or not
 *      this is used to avoid recursive locks when retrieving the context
 *      data in a compare function
 *
 * @return The pointer to the context data
 */
UPS_EXPORT void * UPS_CALLCONV
ups_get_context_data(ups_db_t *db, ups_bool_t dont_lock);

/**
 * Retrieves the Database handle of a Cursor
 *
 * @param cursor A valid Cursor handle
 *
 * @return @a The Database handle of @a cursor
 */
UPS_EXPORT ups_db_t * UPS_CALLCONV
ups_cursor_get_database(ups_cursor_t *cursor);

typedef struct min_max_avg_u32_t {
  uint32_t min;
  uint32_t max;
  uint32_t avg;
  uint32_t _total;      /* for calculating the average */
  uint32_t _instances;  /* for calculating the average */
} min_max_avg_u32_t;

/* btree metrics */
typedef struct btree_metrics_t {
  /* the database name of the btree */
  uint16_t database_name;

  /* number of pages */
  uint64_t number_of_pages;

  /* number of keys */
  uint64_t number_of_keys;

  /* total btree space, including overhead */
  uint64_t total_btree_space;

  /* static overhead per page */
  uint32_t overhead_per_page;

  /* number of keys stored per page (w/o duplicates) */
  min_max_avg_u32_t keys_per_page;

  /* payload storage assigned to the KeyLists */
  min_max_avg_u32_t keylist_ranges;

  /* payload storage assigned to the RecordLists */
  min_max_avg_u32_t recordlist_ranges;

  /* storage assigned to the Indices (if available) */
  min_max_avg_u32_t keylist_index;

  /* storage assigned to the Indices (if available) */
  min_max_avg_u32_t recordlist_index;

  /* unused storage (i.e. gaps between pages, underfilled blocks etc) */
  min_max_avg_u32_t keylist_unused;

  /* unused storage (i.e. gaps between pages, underfilled blocks etc) */
  min_max_avg_u32_t recordlist_unused;

  /* number of blocks per page (if available) */
  min_max_avg_u32_t keylist_blocks_per_page;

  /* block sizes (if available) */
  min_max_avg_u32_t keylist_block_sizes;
} btree_metrics_t;

/**
 * Retrieves collected metrics from the upscaledb Environment. Used mainly
 * for testing.
 * See below for the structure with the currently available metrics.
 * This structure will change a lot; the first field is a version indicator
 * that applications can use to verify that the structure layout is compatible.
 *
 * These metrics are NOT persisted to disk.
 *
 * Metrics marked "global" are stored globally and shared between multiple
 * Environments.
 */
#define UPS_METRICS_VERSION         9

typedef struct ups_env_metrics_t {
  /* the version indicator - must be UPS_METRICS_VERSION */
  uint16_t version;

  /* number of total allocations for the whole lifetime of the process */
  uint64_t mem_total_allocations;

  /* currently active allocations for the whole process */
  uint64_t mem_current_allocations;

  /* current amount of memory allocated and tracked by the process
   * (excludes memory used by the kernel or not allocated with
   * malloc/free) */
  uint64_t mem_current_usage;

  /* peak usage of memory (for the whole process) */
  uint64_t mem_peak_usage;

  /* the heap size of this process */
  uint64_t mem_heap_size;

  /* amount of pages fetched from disk */
  uint64_t page_count_fetched;

  /* amount of pages written to disk */
  uint64_t page_count_flushed;

  /* number of index pages in this Environment */
  uint64_t page_count_type_index;

  /* number of blob pages in this Environment */
  uint64_t page_count_type_blob;

  /* number of page-manager pages in this Environment */
  uint64_t page_count_type_page_manager;

  /* number of successful freelist hits */
  uint64_t freelist_hits;

  /* number of freelist misses */
  uint64_t freelist_misses;

  /* number of successful cache hits */
  uint64_t cache_hits;

  /* number of cache misses */
  uint64_t cache_misses;

  /* number of blobs allocated */
  uint64_t blob_total_allocated;

  /* number of blobs read */
  uint64_t blob_total_read;

  /* (global) number of btree page splits */
  uint64_t btree_smo_split;

  /* (global) number of btree page merges */
  uint64_t btree_smo_merge;

  /* (global) number of extended keys */
  uint64_t extended_keys;

  /* (global) number of extended duplicate tables */
  uint64_t extended_duptables;

  /* number of bytes that the log/journal flushes to disk */
  uint64_t journal_bytes_flushed;

  /* PRO: log/journal bytes before compression */
  uint64_t journal_bytes_before_compression;

  /* PRO: log/journal bytes after compression */
  uint64_t journal_bytes_after_compression;

  /* PRO: record bytes before compression */
  uint64_t record_bytes_before_compression;

  /* PRO: record bytes after compression */
  uint64_t record_bytes_after_compression;

  /* PRO: key bytes before compression */
  uint64_t key_bytes_before_compression;

  /* PRO: key bytes after compression */
  uint64_t key_bytes_after_compression;

  /* PRO: set to the max. SIMD lane width (0 if SIMD is not available) */
  int simd_lane_width;
 
  /* btree metrics for leaf nodes */
  btree_metrics_t btree_leaf_metrics;

  /* btree metrics for internal nodes */
  btree_metrics_t btree_internal_metrics;

  // PRO: set to true if AVX is enabled
  ups_bool_t is_avx_enabled;

} ups_env_metrics_t;

/**
 * Retrieves the current metrics from an Environment
 */
UPS_EXPORT ups_status_t UPS_CALLCONV
ups_env_get_metrics(ups_env_t *env, ups_env_metrics_t *metrics);

/**
 * Returns @ref UPS_TRUE if this upscaledb library was compiled with debug
 * diagnostics, checks and asserts
 */
UPS_EXPORT ups_bool_t UPS_CALLCONV
ups_is_debug();

/**
 * Returns the hash of a compare function name. The name is case-insensitive.
 */
UPS_EXPORT uint32_t UPS_CALLCONV
ups_calc_compare_name_hash(const char *zname);

/**
 * Returns the hash of a compare function name of a database.
 */
UPS_EXPORT uint32_t UPS_CALLCONV
ups_db_get_compare_name_hash(ups_db_t *db);

/**
 * A function which cleans up statically allocated memory.
 *
 * Typically, you do not have to call this function because it is automatically
 * registered as an exit-handler (with at_exit()).
 */
UPS_EXPORT void UPS_CALLCONV
ups_at_exit();

/**
 * @}
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* UPS_UPSCALEDB_INT_H */
