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

#include "3rdparty/catch/catch.hpp"

#include "utils.h"

#include "1base/pickle.h"
#include "2page/page.h"
#include "2device/device.h"
#include "3page_manager/freelist.h"
#include "3page_manager/page_manager.h"
#include "4context/context.h"
#include "4env/env_local.h"
#include "4txn/txn.h"

namespace upscaledb {


struct PageManagerFixture {
  ups_db_t *m_db;
  ups_env_t *m_env;
  bool m_inmemory;
  Device *m_device;
  ScopedPtr<Context> m_context;

  PageManagerFixture(bool inmemorydb = false, uint32_t cachesize = 0)
      : m_db(0), m_inmemory(inmemorydb), m_device(0) {
    uint32_t flags = 0;

    if (m_inmemory)
      flags |= UPS_IN_MEMORY;

    ups_parameter_t params[2] = {{0, 0}, {0, 0}};
    if (cachesize) {
      params[0].name = UPS_PARAM_CACHE_SIZE;
      params[0].value = cachesize;
    }

    REQUIRE(0 ==
        ups_env_create(&m_env, Utils::opath(".test"), flags,
                0644, &params[0]));
    REQUIRE(0 ==
        ups_env_create_db(m_env, &m_db, 1, 0, 0));

    m_context.reset(new Context((LocalEnvironment *)m_env, 0,
                            (LocalDatabase *)m_db));
  }

  ~PageManagerFixture() {
    if (m_env != 0) {
      m_context->changeset.clear();
      REQUIRE(0 == ups_env_close(m_env, UPS_AUTO_CLEANUP));
    }
  }

  void fetchPageTest() {
    PageManager *pm = ((LocalEnvironment *)m_env)->page_manager();
    Page *page;

    page = 0;
    REQUIRE((page = pm->fetch(m_context.get(), 16 * 1024ull)) != 0);
    REQUIRE(page->address() == 16 * 1024ull);

    page = 0;
    REQUIRE((page = pm->fetch(m_context.get(), 16 * 1024ull,
                    PageManager::kOnlyFromCache)) != 0);
    REQUIRE(page->address() == 16 * 1024ull);
    REQUIRE(page != 0);
  }

  void allocPageTest() {
    PageManager *pm = ((LocalEnvironment *)m_env)->page_manager();
    Page *page;

    page = 0;
    REQUIRE((page = pm->alloc(m_context.get(), Page::kTypePageManager,
                            PageManager::kClearWithZero)) != 0);
    if (m_inmemory == false)
      REQUIRE(page->address() == 2 * 16 * 1024ull);
    REQUIRE(page != 0);
    REQUIRE(page->db() == ((LocalDatabase *)m_db));
  }

  void setCacheSizeEnvCreate() {
    REQUIRE(0 == ups_env_close(m_env, UPS_AUTO_CLEANUP));

    ups_db_t *db = 0;
    ups_parameter_t param[] = {
      { UPS_PARAM_CACHE_SIZE, 100 * 1024 },
      { UPS_PARAM_PAGE_SIZE,  1024 },
      { 0, 0 }
    };

    REQUIRE(0 ==
        ups_env_create(&m_env, Utils::opath(".test"),  
            0, 0644, &param[0]));
    REQUIRE(0 ==
        ups_env_create_db(m_env, &db, 13, 0, 0));

    LocalEnvironment *lenv = (LocalEnvironment *)m_env;

    REQUIRE(102400ull == lenv->config().cache_size_bytes);
  }

  void setCacheSizeEnvOpen(uint64_t size) {
    REQUIRE(0 == ups_env_close(m_env, UPS_AUTO_CLEANUP));

    ups_parameter_t param[] = {
      { UPS_PARAM_CACHE_SIZE, size },
      { 0, 0 }
    };

    REQUIRE(0 ==
        ups_env_open(&m_env, Utils::opath(".test"),  
            0, &param[0]));

    LocalEnvironment *lenv = (LocalEnvironment *)m_env;

    REQUIRE(size == lenv->config().cache_size_bytes);
  }

  void cachePutGet() {
    LocalEnvironment *lenv = (LocalEnvironment *)m_env;

    PPageData pers;
    memset(&pers, 0, sizeof(pers));

    Page *page = new Page(lenv->device());
    page->set_address(0x123ull);
    page->set_data(&pers);
    page->set_without_header(true);

    PageManager *pm = lenv->page_manager();

    pm->state->cache.put(page);
    REQUIRE(page == pm->state->cache.get(0x123ull));
    pm->state->cache.del(page);

    page->set_data(0);
    delete page;
  }

  void cachePutGetRemove() {
    LocalEnvironment *lenv = (LocalEnvironment *)m_env;

    PPageData pers;
    memset(&pers, 0, sizeof(pers));

    Page *page = new Page(lenv->device());
    page->set_address(0x123ull);
    page->set_data(&pers);
    page->set_without_header(true);

    PageManager *pm = lenv->page_manager();
    pm->state->cache.put(page);
    REQUIRE(page == pm->state->cache.get(0x123ull));
    pm->state->cache.del(page);
    REQUIRE((Page *)0 == pm->state->cache.get(0x123ull));

    page->set_data(0);
    delete page;
  }

  void cacheManyPuts() {
    LocalEnvironment *lenv = (LocalEnvironment *)m_env;
    Page *page[20];
    PPageData pers[20];
    PageManager *pm = lenv->page_manager();

    for (int i = 0; i < 20; i++) {
      page[i] = new Page(lenv->device());
      memset(&pers[i], 0, sizeof(pers[i]));
      page[i]->set_without_header(true);
      page[i]->set_address(i + 1);
      page[i]->set_data(&pers[i]);
      pm->state->cache.put(page[i]);
    }
    for (int i = 0; i < 20; i++)
      REQUIRE(page[i] == pm->state->cache.get(i + 1));
    for (int i = 0; i < 20; i++)
      pm->state->cache.del(page[i]);
    for (int i = 0; i < 20; i++) {
      REQUIRE((Page *)0 == pm->state->cache.get(i + 1));
      page[i]->set_data(0);
      delete page[i];
    }
  }

  void cacheNegativeGets() {
    LocalEnvironment *lenv = (LocalEnvironment *)m_env;
    PageManager *pm = lenv->page_manager();

    for (int i = 0; i < 20; i++)
      REQUIRE((Page *)0 == pm->state->cache.get(i + 1));
  }

  void cacheFullTest() {
    LocalEnvironment *lenv = (LocalEnvironment *)m_env;
    PageManager *pm = lenv->page_manager();

    PPageData pers;
    memset(&pers, 0, sizeof(pers));
    std::vector<Page *> v;

    for (unsigned int i = 0; i < 15; i++) {
      Page *p = new Page(lenv->device());
      p->set_without_header(true);
      p->assign_allocated_buffer(&pers, i + 1);
      v.push_back(p);
      pm->state->cache.put(p);
      REQUIRE(false == pm->state->cache.is_cache_full());
    }

    for (unsigned int i = 0; i < 5; i++) {
      Page *p = new Page(lenv->device());
      p->set_without_header(true);
      p->assign_allocated_buffer(&pers, i + 15 + 1);
      v.push_back(p);
      pm->state->cache.put(p);
      REQUIRE(true == pm->state->cache.is_cache_full());
    }

    for (unsigned int i = 0; i < 5; i++) {
      REQUIRE(true == pm->state->cache.is_cache_full());
      Page *p = v.back();
      v.pop_back();
      pm->state->cache.del(p);
      p->set_data(0);
      delete p;
    }

    for (unsigned int i = 0; i < 15; i++) {
      Page *p = v.back();
      v.pop_back();
      pm->state->cache.del(p);
      REQUIRE(false == pm->state->cache.is_cache_full());
      p->set_data(0);
      delete p;
    }

    REQUIRE(false == pm->state->cache.is_cache_full());
  }

  void storeStateTest() {
    LocalEnvironment *lenv = (LocalEnvironment *)m_env;
    PageManagerState *state = lenv->page_manager()->state.get();
    uint32_t page_size = lenv->config().page_size_bytes;

    // fill with freelist pages and blob pages
    for (int i = 0; i < 10; i++)
      state->freelist.free_pages[page_size * (i + 100)] = 1;

    state->needs_flush = true;
    REQUIRE(lenv->page_manager()->test_store_state() == page_size * 2);

    // reopen the database
    REQUIRE(0 == ups_env_close(m_env, UPS_AUTO_CLEANUP));
    REQUIRE(0 == ups_env_open(&m_env, Utils::opath(".test"),  0, 0));

    lenv = (LocalEnvironment *)m_env;
    state = lenv->page_manager()->state.get();

    // and check again - the entries must be collapsed
    Freelist::FreeMap::iterator it = state->freelist.free_pages.begin();
    REQUIRE(it->first == page_size * 100);
    REQUIRE(it->second == 10);
  }

  void reclaimTest() {
    LocalEnvironment *lenv = (LocalEnvironment *)m_env;
    PageManager *pm = lenv->page_manager();
    uint32_t page_size = lenv->config().page_size_bytes;
    Page *page[5] = {0};

    // force-flush the state of the PageManager; otherwise it will be
    // written AFTER the allocated pages, and disable the reclaim
    pm->state->needs_flush = true;
    // pretend there is data to write, otherwise test_store_state() is a nop
    pm->state->freelist.free_pages[page_size] = 0;
    pm->test_store_state();
    pm->state->freelist.free_pages.clear(); // clean up again

    // allocate 5 pages
    for (int i = 0; i < 5; i++) {
      REQUIRE((page[i] = pm->alloc(m_context.get(), 0)) != 0);
      REQUIRE(page[i]->address() == (3 + i) * page_size);
    }

    // free the last 3 of them and move them to the freelist (and verify with
    // has())
    for (int i = 2; i < 5; i++) {
      pm->del(m_context.get(), page[i]);
      REQUIRE(true == pm->state->freelist.has(page[i]->address()));
    }
    for (int i = 0; i < 2; i++) {
      REQUIRE(false == pm->state->freelist.has(page[i]->address()));
    }

    // verify file size
    REQUIRE((uint64_t)(page_size * 8) == lenv->device()->file_size());

    // reopen the file
    m_context->changeset.clear();
    REQUIRE(0 == ups_env_close(m_env, UPS_AUTO_CLEANUP));
    REQUIRE(0 == ups_env_open(&m_env, Utils::opath(".test"),  0, 0));
    m_context.reset(new Context((LocalEnvironment *)m_env, 0,
                            (LocalDatabase *)m_db));

    lenv = (LocalEnvironment *)m_env;
    pm = lenv->page_manager();

    for (int i = 0; i < 2; i++)
      REQUIRE(false == pm->state->freelist.has((3 + i) * page_size));

    // verify file size
#ifndef WIN32
    REQUIRE((uint64_t)(page_size * 5) == lenv->device()->file_size());
#endif
  }

  void issue60Test() {
#ifndef WIN32
    const char *foo = "123456789012345567890123456789012345678901234567890";

    REQUIRE(0 == ups_env_close(m_env, UPS_AUTO_CLEANUP));
    REQUIRE(0 == ups_env_create(&m_env, Utils::opath(".test"), 0, 0644, 0));
    REQUIRE(0 == ups_env_create_db(m_env, &m_db, 1, 0, 0));

    for (int i = 0; i < 50000; i++) {
      ups_key_t key = ups_make_key(&i, sizeof(i));
      ups_record_t rec = ups_make_record((void *)foo, (uint32_t)strlen(foo));
      REQUIRE(0 == ups_db_insert(m_db, 0, &key, &rec, 0));
    }

    for (int i = 0; i < 50000; i++) {
      ups_key_t key = ups_make_key(&i, sizeof(i));
      REQUIRE(0 == ups_db_erase(m_db, 0, &key, 0));
    }

    REQUIRE(0 == ups_db_close(m_db, 0));
    REQUIRE(0 == ups_env_erase_db(m_env, 1, 0));
    REQUIRE(0 == ups_env_close(m_env, UPS_AUTO_CLEANUP));
    m_env = 0;

    File f;
    f.open(".test", false);
    REQUIRE(f.file_size() == 1024 * 16);
    f.close();
#endif
  }

  void collapseFreelistTest() {
    LocalEnvironment *lenv = (LocalEnvironment *)m_env;
    PageManager *pm = lenv->page_manager();
    uint32_t page_size = lenv->config().page_size_bytes;

    for (int i = 1; i <= 150; i++)
      pm->state->freelist.free_pages[page_size * i] = 1;

    // store the state on disk
    pm->state->needs_flush = true;
    uint64_t page_id = pm->test_store_state();

    pm->flush_all_pages();
    pm->state->freelist.free_pages.clear();

    pm->initialize(page_id);

    REQUIRE(10 == pm->state->freelist.free_pages.size());
    for (int i = 1; i < 10; i++)
      REQUIRE(pm->state->freelist.free_pages[page_size * (1 + i * 15)] == 15);
  }

  void encodeDecodeTest() {
    uint8_t buffer[32] = {0};

    for (int i = 1; i < 10000; i++) {
      int num_bytes = Pickle::encode_u64(&buffer[0], i * 13);
      REQUIRE(Pickle::decode_u64(num_bytes, &buffer[0]) == (uint64_t)i * 13);
    }
  }

  void storeBigStateTest() {
    LocalEnvironment *lenv = (LocalEnvironment *)m_env;
    PageManager *pm = lenv->page_manager();
    uint32_t page_size = lenv->config().page_size_bytes;

    pm->state->last_blob_page_id = page_size * 100;

    for (int i = 1; i <= 30000; i++) {
      if (i & 1) // only store every 2nd page to avoid collapsing
        pm->state->freelist.free_pages[page_size * i] = 1;
    }

    // store the state on disk
    pm->state->needs_flush = true;
    uint64_t page_id = pm->test_store_state();

    pm->flush_all_pages();
    pm->state->freelist.free_pages.clear();
    pm->state->last_blob_page_id = 0;

    pm->initialize(page_id);

    REQUIRE(pm->state->last_blob_page_id == page_size * 100);

    REQUIRE(15000 == pm->state->freelist.free_pages.size());
    for (int i = 1; i <= 30000; i++) {
      if (i & 1)
        REQUIRE(pm->state->freelist.free_pages[page_size * i] == 1);
    }

    REQUIRE(pm->state->page_count_page_manager == 4u);
  }

  void allocMultiBlobs() {
    LocalEnvironment *lenv = (LocalEnvironment *)m_env;
    PageManager *pm = lenv->page_manager();
    uint32_t page_size = lenv->config().page_size_bytes;

    Context context(lenv, 0, 0);

    Page *head = pm->alloc_multiple_blob_pages(&context, 10);
    REQUIRE(head != 0);
    pm->del(&context, head, 10);

    Page *page1 = pm->alloc_multiple_blob_pages(&context, 2);
    REQUIRE(page1 != 0);
    REQUIRE(page1->address() == head->address());

    Page *page2 = pm->alloc_multiple_blob_pages(&context, 8);
    REQUIRE(page2 != 0);
    REQUIRE(page2->address() == page1->address() + page_size * 2);
  }
};

TEST_CASE("PageManager/fetchPage", "")
{
  PageManagerFixture f;
  f.fetchPageTest();
}

TEST_CASE("PageManager/allocPage", "")
{
  PageManagerFixture f;
  f.allocPageTest();
}

TEST_CASE("PageManager/setCacheSizeEnvCreate", "")
{
  PageManagerFixture f;
  f.setCacheSizeEnvCreate();
}

TEST_CASE("PageManager/setCacheSizeEnvOpen", "")
{
  PageManagerFixture f;
  f.setCacheSizeEnvOpen(100 * 1024);
}

TEST_CASE("PageManager/setBigCacheSizeEnvOpen", "")
{
  PageManagerFixture f;
  f.setCacheSizeEnvOpen(1024ull * 1024ull * 1024ull * 16ull);
}

TEST_CASE("PageManager/cachePutGet", "")
{
  PageManagerFixture f;
  f.cachePutGet();
}

TEST_CASE("PageManager/cachePutGetRemove", "")
{
  PageManagerFixture f;
  f.cachePutGetRemove();
}

TEST_CASE("PageManager/cacheManyPuts", "")
{
  PageManagerFixture f;
  f.cacheManyPuts();
}

TEST_CASE("PageManager/cacheNegativeGets", "")
{
  PageManagerFixture f;
  f.cacheNegativeGets();
}

TEST_CASE("PageManager/cacheFullTest", "")
{
  PageManagerFixture f(false, 16 * UPS_DEFAULT_PAGE_SIZE);
  f.cacheFullTest();
}

TEST_CASE("PageManager/storeStateTest", "")
{
  PageManagerFixture f(false, 16 * UPS_DEFAULT_PAGE_SIZE);
  f.storeStateTest();
}

TEST_CASE("PageManager/reclaimTest", "")
{
  PageManagerFixture f(false, 16 * UPS_DEFAULT_PAGE_SIZE);
  f.reclaimTest();
}

TEST_CASE("PageManager/issue60Test", "")
{
  PageManagerFixture f(false, 16 * UPS_DEFAULT_PAGE_SIZE);
  f.issue60Test();
}

TEST_CASE("PageManager/collapseFreelistTest", "")
{
  PageManagerFixture f(false);
  f.collapseFreelistTest();
}

TEST_CASE("PageManager/encodeDecodeTest", "")
{
  PageManagerFixture f(false);
  f.encodeDecodeTest();
}

TEST_CASE("PageManager/storeBigStateTest", "")
{
  PageManagerFixture f(false);
  f.storeBigStateTest();
}

TEST_CASE("PageManager/allocMultiBlobs", "")
{
  PageManagerFixture f(false);
  f.allocMultiBlobs();
}

TEST_CASE("PageManager-inmem/allocPage", "")
{
  PageManagerFixture f(true);
  f.allocPageTest();
}

} // namespace upscaledb
