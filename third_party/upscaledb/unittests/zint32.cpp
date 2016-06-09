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

#include <vector>
#include <algorithm>

#include <ups/upscaledb_uqi.h>

#include "3rdparty/catch/catch.hpp"
#include "3rdparty/simdcomp/include/simdcomp.h"

#include "1os/os.h"

#include "utils.h"
#include "os.hpp"

namespace upscaledb {

struct Zint32Fixture {
  ups_db_t *m_db;
  ups_env_t *m_env;

  typedef std::vector<uint32_t> IntVector;

  Zint32Fixture(uint64_t compressor, bool use_duplicates, uint64_t record_size)
    : m_db(0), m_env(0) {
    ups_parameter_t p[] = {
      { UPS_PARAM_RECORD_SIZE, record_size },
      { UPS_PARAM_KEY_TYPE, UPS_TYPE_UINT32 },
      { UPS_PARAM_KEY_COMPRESSION, compressor },
      { 0, 0 }
    };

    if (compressor == 0) {
      p[2].name = 0;
      p[2].value = 0;
    }

    REQUIRE(0 == ups_env_create(&m_env, Utils::opath(".test"), 0, 0644, 0));
    REQUIRE(0 == ups_env_create_db(m_env, &m_db, 1,
                    use_duplicates ? UPS_ENABLE_DUPLICATES : 0,
                    &p[0]));
  }

  ~Zint32Fixture() {
    if (m_env)
	  REQUIRE(0 == ups_env_close(m_env, UPS_AUTO_CLEANUP));
  }

  void basicSimdcompTest() {
#ifdef HAVE_SSE2
#ifdef WIN32
    uint32_t *din = (uint32_t *)::_aligned_malloc(sizeof(uint32_t) * 128, 16);
#else
	uint32_t *din = (uint32_t *)::malloc(sizeof(uint32_t) * 128);
#endif
    for (uint32_t i = 0; i < 128; i++)
      din[i] = i;

    uint32_t dout[128];
    uint32_t bits = simdmaxbitsd1(0, &din[0]);
    REQUIRE(bits == 1);
    simdpackwithoutmaskd1(0, &din[0], (__m128i *)&dout[0], bits);

    ::memset(&din[0], 0, sizeof(uint32_t) * 128);
    simdunpackd1(0, (__m128i *)&dout[0], &din[0], bits);

    for (uint32_t i = 0; i < 128; i++)
      REQUIRE(din[i] == i);

#ifdef WIN32
	::_aligned_free(din);
#else
	::free(din);
#endif
#endif // HAVE_SSE2
  }

  void insertFindEraseFind(const IntVector &ivec) {
    ups_key_t key = {0};
    ups_record_t record = {0};

    for (IntVector::const_iterator it = ivec.begin(); it != ivec.end(); it++) {
      uint32_t k = *it;
      key.data = (void *)&k;
      key.size = sizeof(k);
      record.data = (void *)&k;
      record.size = sizeof(k);

      REQUIRE(0 == ups_db_insert(m_db, 0, &key, &record, 0));
    }

    for (IntVector::const_iterator it = ivec.begin();
                    it != ivec.end(); it++) {
      uint32_t k = *it;
      key.data = (void *)&k;
      key.size = sizeof(k);

      REQUIRE(0 == ups_db_find(m_db, 0, &key, &record, 0));
      REQUIRE(record.size == sizeof(uint32_t));
      REQUIRE(*(uint32_t *)record.data == k);
    }

    for (IntVector::const_iterator it = ivec.begin();
                    it != ivec.end(); it++) {
      uint32_t k = *it;
      key.data = (void *)&k;
      key.size = sizeof(k);

      REQUIRE(0 == ups_db_erase(m_db, 0, &key, 0));
    }

    for (IntVector::const_iterator it = ivec.begin();
                    it != ivec.end(); it++) {
      uint32_t k = *it;
      key.data = (void *)&k;
      key.size = sizeof(k);

      REQUIRE(UPS_KEY_NOT_FOUND == ups_db_find(m_db, 0, &key, &record, 0));
    }
  }

  void uqiTest() {
    ups_key_t key = {0};
    ups_record_t record = {0};

    for (uint32_t i = 0; i < 30000; i++) {
      key.data = (void *)&i;
      key.size = sizeof(i);

      REQUIRE(0 == ups_db_insert(m_db, 0, &key, &record, 0));
    }

    uqi_result_t *result;
    uint32_t size;

    REQUIRE(0 == uqi_select(m_env, "SUM($key) from database 1", &result));
    REQUIRE(uqi_result_get_record_type(result) == UPS_TYPE_UINT64);
    REQUIRE(*(uint64_t *)uqi_result_get_record_data(result, &size) == 449985000ull);

    uqi_result_close(result);

    REQUIRE(0 == uqi_select(m_env, "AVERAGE($key) from database 1", &result));
    REQUIRE(uqi_result_get_record_type(result) == UPS_TYPE_REAL64);
    REQUIRE(*(double *)uqi_result_get_record_data(result, &size) == 14999.5);

    uqi_result_close(result);
  }

  void uqiTestDuplicate() {
    ups_key_t key = {0};
    ups_record_t record = {0};

    uint32_t max = 10000;
    for (uint32_t i = 0; i < max; i++) {
      key.data = (void *)&i;
      key.size = sizeof(i);

      REQUIRE(0 == ups_db_insert(m_db, 0, &key, &record, UPS_DUPLICATE));
      REQUIRE(0 == ups_db_insert(m_db, 0, &key, &record, UPS_DUPLICATE));
      REQUIRE(0 == ups_db_insert(m_db, 0, &key, &record, UPS_DUPLICATE));
    }

    uint64_t value;
    uint32_t size;
    uqi_result_t *result;

    //REQUIRE(0 == ups_db_count(m_db, 0, 0, &value));
    //REQUIRE(30000ull == value);

    REQUIRE(0 == uqi_select(m_env, "COUNT ($key) from database 1",
                        &result));
    REQUIRE(uqi_result_get_record_type(result) == UPS_TYPE_UINT64);
    value = *(uint64_t *)uqi_result_get_record_data(result, &size);
    REQUIRE(value == max * 3);
    REQUIRE(size == 8ull);
    uqi_result_close(result);

    REQUIRE(0 == uqi_select(m_env, "DISTINCT COUNT ($key) from database 1",
                        &result));
    REQUIRE(uqi_result_get_record_type(result) == UPS_TYPE_UINT64);
    value = *(uint64_t *)uqi_result_get_record_data(result, &size);
    REQUIRE(value == max);
    REQUIRE(size == 8ull);
    uqi_result_close(result);
  }
};

TEST_CASE("Zint32/Pod/randomDataTest", "")
{
  Zint32Fixture::IntVector ivec;
  for (int i = 0; i < 30000; i++)
    ivec.push_back(i);
  std::srand(0); // make this reproducable
  std::random_shuffle(ivec.begin(), ivec.end());

  Zint32Fixture f(0, false, 4);
  f.insertFindEraseFind(ivec);
}

TEST_CASE("Zint32/Pod/ascendingDataTest", "")
{
  Zint32Fixture::IntVector ivec;
  for (int i = 0; i < 30000; i++)
    ivec.push_back(i);

  Zint32Fixture f(0, false, 4);
  f.insertFindEraseFind(ivec);
}

TEST_CASE("Zint32/Pod/descendingDataTest", "")
{
  Zint32Fixture::IntVector ivec;
  for (int i = 30000; i >= 0; i--)
    ivec.push_back(i);

  Zint32Fixture f(0, false, 4);
  f.insertFindEraseFind(ivec);
}

TEST_CASE("Zint32/Pod/uqiTest", "")
{
  Zint32Fixture f(0, false, 0);
  f.uqiTest();
}

TEST_CASE("Zint32/Pod/uqiTest-duplicate", "")
{
  Zint32Fixture f(0, true, 0);
  f.uqiTestDuplicate();
}

TEST_CASE("Zint32/Varbyte/randomDataTest", "")
{
  Zint32Fixture::IntVector ivec;
  for (int i = 0; i < 30000; i++)
    ivec.push_back(i);
  std::srand(0); // make this reproducable
  std::random_shuffle(ivec.begin(), ivec.end());

  Zint32Fixture f(UPS_COMPRESSOR_UINT32_VARBYTE, false, 4);
  f.insertFindEraseFind(ivec);
}

TEST_CASE("Zint32/Varbyte/ascendingDataTest", "")
{
  Zint32Fixture::IntVector ivec;
  for (int i = 0; i < 30000; i++)
    ivec.push_back(i);

  Zint32Fixture f(UPS_COMPRESSOR_UINT32_VARBYTE, false, 4);
  f.insertFindEraseFind(ivec);
}

TEST_CASE("Zint32/Varbyte/descendingDataTest", "")
{
  Zint32Fixture::IntVector ivec;
  for (int i = 30000; i >= 0; i--)
    ivec.push_back(i);

  Zint32Fixture f(UPS_COMPRESSOR_UINT32_VARBYTE, false, 4);
  f.insertFindEraseFind(ivec);
}

TEST_CASE("Zint32/Varbyte/uqiTest", "")
{
  Zint32Fixture f(UPS_COMPRESSOR_UINT32_VARBYTE, false, 0);
  f.uqiTest();
}

TEST_CASE("Zint32/Varbyte/uqiTest-duplicate", "")
{
  Zint32Fixture f(UPS_COMPRESSOR_UINT32_VARBYTE, true, 0);
  f.uqiTestDuplicate();
}

TEST_CASE("Zint32/SimdComp/basicSimdcompTest", "")
{
#ifdef HAVE_SSE2
  Zint32Fixture f(UPS_COMPRESSOR_UINT32_SIMDCOMP, false, 4);
  f.basicSimdcompTest();
#endif
}

TEST_CASE("Zint32/SimdComp/randomDataTest", "")
{
#ifdef HAVE_SSE2
  Zint32Fixture::IntVector ivec;
  for (int i = 0; i < 30000; i++)
    ivec.push_back(i);
  std::srand(0); // make this reproducible
  std::random_shuffle(ivec.begin(), ivec.end());

  Zint32Fixture f(UPS_COMPRESSOR_UINT32_SIMDCOMP, false, 4);
  f.insertFindEraseFind(ivec);
#endif
}

TEST_CASE("Zint32/SimdComp/ascendingDataTest", "")
{
#ifdef HAVE_SSE2
  Zint32Fixture::IntVector ivec;
  for (int i = 0; i < 30000; i++)
    ivec.push_back(i);

  Zint32Fixture f(UPS_COMPRESSOR_UINT32_SIMDCOMP, false, 4);
  f.insertFindEraseFind(ivec);
#endif
}

TEST_CASE("Zint32/SimdComp/descendingDataTest", "")
{
#ifdef HAVE_SSE2
  Zint32Fixture::IntVector ivec;
  for (int i = 30000; i >= 0; i--)
    ivec.push_back(i);

  Zint32Fixture f(UPS_COMPRESSOR_UINT32_SIMDCOMP, false, 4);
  f.insertFindEraseFind(ivec);
#endif
}

TEST_CASE("Zint32/SimdComp/uqiTest", "")
{
#ifdef HAVE_SSE2
  Zint32Fixture f(UPS_COMPRESSOR_UINT32_SIMDCOMP, false, 0);
  f.uqiTest();
#endif
}

TEST_CASE("Zint32/SimdComp/uqiTest-duplicate", "")
{
#ifdef HAVE_SSE2
  Zint32Fixture f(UPS_COMPRESSOR_UINT32_SIMDCOMP, true, 0);
  f.uqiTestDuplicate();
#endif
}

TEST_CASE("Zint32/GroupVarint/randomDataTest", "")
{
#ifdef HAVE_SSE2
  Zint32Fixture::IntVector ivec;
  for (int i = 0; i < 30000; i++)
    ivec.push_back(i);
  std::srand(0); // make this reproducible
  std::random_shuffle(ivec.begin(), ivec.end());

  Zint32Fixture f(UPS_COMPRESSOR_UINT32_GROUPVARINT, false, 4);
  f.insertFindEraseFind(ivec);
#endif
}

TEST_CASE("Zint32/GroupVarint/ascendingDataTest", "")
{
#ifdef HAVE_SSE2
  Zint32Fixture::IntVector ivec;
  for (int i = 0; i < 30000; i++)
    ivec.push_back(i);

  Zint32Fixture f(UPS_COMPRESSOR_UINT32_GROUPVARINT, false, 4);
  f.insertFindEraseFind(ivec);
#endif
}

TEST_CASE("Zint32/GroupVarint/descendingDataTest", "")
{
#ifdef HAVE_SSE2
  Zint32Fixture::IntVector ivec;
  for (int i = 30000; i >= 0; i--)
    ivec.push_back(i);

  Zint32Fixture f(UPS_COMPRESSOR_UINT32_GROUPVARINT, false, 4);
  f.insertFindEraseFind(ivec);
#endif
}

TEST_CASE("Zint32/GroupVarint/uqiTest", "")
{
#ifdef HAVE_SSE2
  Zint32Fixture f(UPS_COMPRESSOR_UINT32_GROUPVARINT, false, 0);
  f.uqiTest();
#endif
}

TEST_CASE("Zint32/GroupVarint/uqiTest-duplicate", "")
{
#ifdef HAVE_SSE2
  Zint32Fixture f(UPS_COMPRESSOR_UINT32_GROUPVARINT, true, 0);
  f.uqiTestDuplicate();
#endif
}

TEST_CASE("Zint32/StreamVbyte/randomDataTest", "")
{
#ifdef HAVE_SSE2
  Zint32Fixture::IntVector ivec;
  for (int i = 0; i < 30000; i++)
    ivec.push_back(i);
  std::srand(0); // make this reproducible
  std::random_shuffle(ivec.begin(), ivec.end());

  Zint32Fixture f(UPS_COMPRESSOR_UINT32_STREAMVBYTE, false, 4);
  f.insertFindEraseFind(ivec);
#endif
}

TEST_CASE("Zint32/StreamVbyte/ascendingDataTest", "")
{
#ifdef HAVE_SSE2
  Zint32Fixture::IntVector ivec;
  for (int i = 0; i < 30000; i++)
    ivec.push_back(i);

  Zint32Fixture f(UPS_COMPRESSOR_UINT32_STREAMVBYTE, false, 4);
  f.insertFindEraseFind(ivec);
#endif
}

TEST_CASE("Zint32/StreamVbyte/descendingDataTest", "")
{
#ifdef HAVE_SSE2
  Zint32Fixture::IntVector ivec;
  for (int i = 30000; i >= 0; i--)
    ivec.push_back(i);

  Zint32Fixture f(UPS_COMPRESSOR_UINT32_STREAMVBYTE, false, 4);
  f.insertFindEraseFind(ivec);
#endif
}

TEST_CASE("Zint32/StreamVbyte/uqiTest", "")
{
#ifdef HAVE_SSE2
  Zint32Fixture f(UPS_COMPRESSOR_UINT32_STREAMVBYTE, false, 0);
  f.uqiTest();
#endif
}

TEST_CASE("Zint32/StreamVbyte/uqiTest-duplicate", "")
{
#ifdef HAVE_SSE2
  Zint32Fixture f(UPS_COMPRESSOR_UINT32_STREAMVBYTE, true, 0);
  f.uqiTestDuplicate();
#endif
}

TEST_CASE("Zint32/MaskedVbyte/randomDataTest", "")
{
  if (os_has_avx()) {
    Zint32Fixture::IntVector ivec;
    for (int i = 0; i < 30000; i++)
      ivec.push_back(i);
    std::srand(0); // make this reproducible
    std::random_shuffle(ivec.begin(), ivec.end());

    Zint32Fixture f(UPS_COMPRESSOR_UINT32_MASKEDVBYTE, false, 4);
    f.insertFindEraseFind(ivec);
  }
}

TEST_CASE("Zint32/MaskedVbyte/ascendingDataTest", "")
{
  if (os_has_avx()) {
    Zint32Fixture::IntVector ivec;
    for (int i = 0; i < 30000; i++)
      ivec.push_back(i);

    Zint32Fixture f(UPS_COMPRESSOR_UINT32_MASKEDVBYTE, false, 4);
    f.insertFindEraseFind(ivec);
  }
}

TEST_CASE("Zint32/MaskedVbyte/descendingDataTest", "")
{
  if (os_has_avx()) {
    Zint32Fixture::IntVector ivec;
    for (int i = 30000; i >= 0; i--)
      ivec.push_back(i);

    Zint32Fixture f(UPS_COMPRESSOR_UINT32_MASKEDVBYTE, false, 4);
    f.insertFindEraseFind(ivec);
  }
}

TEST_CASE("Zint32/MaskedVbyte/uqiTest", "")
{
#ifdef HAVE_SSE2
  Zint32Fixture f(UPS_COMPRESSOR_UINT32_MASKEDVBYTE, false, 0);
  f.uqiTest();
#endif
}

TEST_CASE("Zint32/MaskedVbyte/uqiTest-duplicate", "")
{
#ifdef HAVE_SSE2
  Zint32Fixture f(UPS_COMPRESSOR_UINT32_MASKEDVBYTE, true, 0);
  f.uqiTestDuplicate();
#endif
}

TEST_CASE("Zint32/FOR/randomDataTest", "")
{
  Zint32Fixture::IntVector ivec;
  for (int i = 0; i < 30000; i++)
    ivec.push_back(i);
  std::srand(0); // make this reproducible
  std::random_shuffle(ivec.begin(), ivec.end());

  Zint32Fixture f(UPS_COMPRESSOR_UINT32_FOR, false, 4);
  f.insertFindEraseFind(ivec);
}

TEST_CASE("Zint32/FOR/ascendingDataTest", "")
{
  Zint32Fixture::IntVector ivec;
  for (int i = 0; i < 30000; i++)
    ivec.push_back(i);

  Zint32Fixture f(UPS_COMPRESSOR_UINT32_FOR, false, 4);
  f.insertFindEraseFind(ivec);
}

TEST_CASE("Zint32/FOR/descendingDataTest", "")
{
  Zint32Fixture::IntVector ivec;
  for (int i = 30000; i >= 0; i--)
    ivec.push_back(i);

  Zint32Fixture f(UPS_COMPRESSOR_UINT32_FOR, false, 4);
  f.insertFindEraseFind(ivec);
}

TEST_CASE("Zint32/FOR/uqiTest", "")
{
  Zint32Fixture f(UPS_COMPRESSOR_UINT32_FOR, false, 0);
  f.uqiTest();
}

TEST_CASE("Zint32/FOR/uqiTest-duplicate", "")
{
  Zint32Fixture f(UPS_COMPRESSOR_UINT32_FOR, true, 0);
  f.uqiTestDuplicate();
}

TEST_CASE("Zint32/SimdFOR/randomDataTest", "")
{
#ifdef HAVE_SSE2
  Zint32Fixture::IntVector ivec;
  for (int i = 0; i < 30000; i++)
    ivec.push_back(i);
  std::srand(0); // make this reproducible
  std::random_shuffle(ivec.begin(), ivec.end());

  Zint32Fixture f(UPS_COMPRESSOR_UINT32_SIMDFOR, false, 4);
  f.insertFindEraseFind(ivec);
#endif
}

TEST_CASE("Zint32/SimdFOR/ascendingDataTest", "")
{
#ifdef HAVE_SSE2
  Zint32Fixture::IntVector ivec;
  for (int i = 0; i < 30000; i++)
    ivec.push_back(i);

  Zint32Fixture f(UPS_COMPRESSOR_UINT32_SIMDFOR, false, 4);
  f.insertFindEraseFind(ivec);
#endif
}

TEST_CASE("Zint32/SimdFOR/descendingDataTest", "")
{
#ifdef HAVE_SSE2
  Zint32Fixture::IntVector ivec;
  for (int i = 30000; i >= 0; i--)
    ivec.push_back(i);

  Zint32Fixture f(UPS_COMPRESSOR_UINT32_SIMDFOR, false, 4);
  f.insertFindEraseFind(ivec);
#endif
}

TEST_CASE("Zint32/SimdFOR/uqiTest", "")
{
#ifdef HAVE_SSE2
  Zint32Fixture f(UPS_COMPRESSOR_UINT32_SIMDFOR, false, 0);
  f.uqiTest();
#endif
}

TEST_CASE("Zint32/SimdFOR/uqiTest-duplicate", "")
{
#ifdef HAVE_SSE2
  Zint32Fixture f(UPS_COMPRESSOR_UINT32_SIMDFOR, true, 0);
  f.uqiTestDuplicate();
#endif
}

TEST_CASE("Zint32/Zint32/invalidPagesizeTest", "")
{
  ups_parameter_t p1[] = {
    { UPS_PARAM_PAGE_SIZE, 1024 },
    { 0, 0 }
  };
  ups_parameter_t p2[] = {
    { UPS_PARAM_KEY_TYPE, UPS_TYPE_UINT32 },
    { UPS_PARAM_KEY_COMPRESSION, UPS_COMPRESSOR_UINT32_VARBYTE},
    { 0, 0 }
  };

  ups_env_t *env;
  ups_db_t *db;

  REQUIRE(0 == ups_env_create(&env, Utils::opath(".test"), 0, 0644, &p1[0]));
  REQUIRE(UPS_INV_PARAMETER == ups_env_create_db(env, &db, 1, 0, &p2[0]));
  ups_env_close(env, 0);
}

} // namespace upscaledb
