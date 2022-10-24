#include "../include/database.h"

#include <city.h>
#include <gtest/gtest.h>

#include <cstdint>
#include <iostream>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

#include "../include/status.h"
#include "../include/storageengine.h"
#include "../include/utils.h"

// self-define K-V size, and record size test.
// TODO: remember change func name.
void put_get_test(int loops, int key_size, int val_size) {
  minikv::DatabaseOptions database_option;
  minikv::Database db(database_option, "/tmp/minikv");

  std::string key;
  std::string val;
  std::string val_place_holder;

  minikv::PutOption put_option;
  minikv::GetOption get_option;

  minikv::Status s;

  for (auto i = 0; i < loops; i++) {
    if (loops > 1000) std::cout << "\riter = " << i + 1 << " " << std::flush;
    key = minikv::Utils::GenRandString(key_size);
    val = minikv::Utils::GenRandString(val_size);

    s = db.Put(put_option, key, val);
    EXPECT_EQ(s.GetCode(), minikv::STATUS_OKAY);
    s = db.Get(get_option, key, &val_place_holder);
    EXPECT_EQ(s.GetCode(), minikv::STATUS_OKAY);
    EXPECT_EQ(val, val_place_holder);
    key.clear();
    val.clear();
    val_place_holder.clear();
  }
  if (loops > 1000) std::cout << std::endl;
}

// overflow test.
void overflow_test(int key_size, int val_size, minikv::StatusCode code) {
  minikv::DatabaseOptions database_option;
  minikv::Database db(database_option, "/tmp/minikv");

  std::string key;
  std::string val;
  std::string val_place_holder;

  minikv::PutOption put_option;

  minikv::Status s;

  key = minikv::Utils::GenRandString(key_size);
  val = minikv::Utils::GenRandString(val_size);

  s = db.Put(put_option, key, val);
  EXPECT_EQ(s.GetCode(), code);
}

void put_then_get(int len, int key_size, int val_size) {
  std::string key;
  std::string val;
  std::string val_place_holder;

  minikv::PutOption put_option;
  minikv::GetOption get_option;
  minikv::Status s;

  // vector store rand gen kv;
  std::vector<std::pair<std::string, std::string>> data;
  // rand gen kv.
  for (auto i = 0; i < len; ++i) {
    key = minikv::Utils::GenRandString(key_size);
    val = minikv::Utils::GenRandString(val_size);
    data.push_back(std::make_pair(key, val));
  }

  minikv::DatabaseOptions database_option;
  minikv::Database db(database_option, "/tmp/minikv");

  for (auto i = 0; i < len; ++i) {
    s = db.Put(put_option, data.at(i).first, data.at(i).second);
    EXPECT_EQ(s.GetCode(), minikv::STATUS_OKAY);
  }

  for (auto i = 0; i < len; ++i) {
    s = db.Get(get_option, data.at(i).first, &val_place_holder);
    EXPECT_EQ(s.GetCode(), minikv::STATUS_OKAY);
    EXPECT_EQ(data.at(i).second, val_place_holder);
  }
}

/*
`test_name` name spec:
1、`test_loops_<loops>_key_<key_length>_val_<val_length>`

2、`test_key_overflow`

3、`test_val_overflow`

4、`test_k_v_overflow`

*/

///////////////////////////////////////////////////////////////////////////////////////////

// BUG: when testing different size key and val, error occur when key andf value
// size too small, such as when key size==1 ,and  value size == 1, lead to
// error. i think value_place_holder maybe wrong ,cause i pass std::string's
// address, and it may change.
// FIXME: !!!!!!!!!!!!

TEST(database_test, kv_sz_1) {
  // put_get_test(1, 1, 1);
  put_get_test(10000, 5, 5);
}

TEST(database_test, put_and_get) {
  minikv::DatabaseOptions db_option;
  auto max_key_sz = db_option._max_key_size;
  auto max_val_sz = db_option._max_val_size;

  put_get_test(1, 1, 1);
  put_get_test(1, 5, 5);
  put_get_test(1, 10, 10);
  put_get_test(1, 50, 50);
  put_get_test(1, 100, 100);
  put_get_test(1, max_key_sz, max_val_sz);

  put_get_test(10, 1, 1);
  put_get_test(10, 5, 5);
  put_get_test(10, 10, 10);
  put_get_test(10, 50, 50);
  put_get_test(10, 100, 100);
  put_get_test(10, max_key_sz, max_val_sz);

  put_get_test(100, 1, 1);
  put_get_test(100, 5, 5);
  put_get_test(100, 10, 10);
  put_get_test(100, 50, 50);
  put_get_test(100, 100, 100);
  put_get_test(100, max_key_sz, max_val_sz);

  put_get_test(1000, 1, 1);
  put_get_test(1000, 5, 5);
  put_get_test(1000, 10, 10);
  put_get_test(1000, 50, 50);
  put_get_test(1000, 100, 100);
  put_get_test(1000, max_key_sz, max_val_sz);

  put_get_test(10000, 1, 1);
  put_get_test(10000, 5, 5);
  put_get_test(10000, 10, 10);
  put_get_test(10000, 50, 50);
  put_get_test(10000, 100, 100);
  put_get_test(10000, max_key_sz, max_val_sz);
}

TEST(database_test, test_key_overflow) {
  minikv::DatabaseOptions database_option;
  overflow_test(database_option._max_key_size + 1,
                database_option._max_val_size, minikv::STATUS_KEY_TOO_LONG);
}

TEST(database_test, test_val_overflow) {
  minikv::DatabaseOptions database_option;
  overflow_test(database_option._max_key_size,
                database_option._max_val_size + 1, minikv::STATUS_VAL_TOO_LONG);
}

TEST(database_test, test_k_v_overflow) {
  minikv::DatabaseOptions database_option;
  overflow_test(database_option._max_key_size + 1,
                database_option._max_val_size + 1, minikv::STATUS_KEY_TOO_LONG);
}

TEST(database_test, put_then_get) {
  minikv::DatabaseOptions db_option;
  auto max_key_sz = db_option._max_key_size;
  auto max_val_sz = db_option._max_val_size;
  // auto max_mmap_sz = db_option._max_single_file_size;

  put_then_get(100, 1, 1);
  put_then_get(100, 10, 10);
  put_then_get(100, 100, 100);
  put_then_get(100, max_key_sz, max_val_sz);

  put_then_get(200, 1, 1);
  put_then_get(200, 10, 10);
  put_then_get(200, 100, 100);
  put_then_get(200, max_key_sz, max_val_sz);
}
