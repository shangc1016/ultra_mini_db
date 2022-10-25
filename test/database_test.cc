#include "../include/database.h"

#include <city.h>
#include <gtest/gtest.h>

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <map>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

#include "../include/status.h"
#include "../include/storageengine.h"
#include "../include/utils.h"

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

// gen legal test case
void gen_test_case(int loops, int key_size, int val_size,
                   std::vector<std::pair<std::string, std::string>> &put_data,
                   std::vector<std::pair<std::string, std::string>> &get_data) {
  put_data.clear();
  get_data.clear();

  std::string key;
  std::string val;
  // step1: gen put_data;
  for (auto i = 0; i < loops; i++) {
    key = minikv::Utils::GenRandString(key_size);
    val = minikv::Utils::GenRandString(val_size);
    put_data.push_back(std::make_pair(key, val));
  }
  // step2: for same key, rm old val, map just do the job.
  std::map<std::string, std::string> map;
  // rm dup key.
  for (auto item : put_data) map[item.first] = item.second;

  // step3: gen get_data
  for (auto item : put_data) {
    auto key = item.first;
    auto val = map[key];
    get_data.push_back(std::make_pair(key, val));
  }
}

// first `put` all items, then `get`.
void put_then_get(std::vector<std::pair<std::string, std::string>> &put_data,
                  std::vector<std::pair<std::string, std::string>> &get_data) {
  minikv::DatabaseOptions database_option;
  minikv::Database db(database_option, "/tmp/minikv");

  std::string val_place_holder;

  minikv::PutOption put_option;
  minikv::GetOption get_option;

  minikv::Status s;
  // step1: put all item;
  for (auto item : put_data) {
    s = db.Put(put_option, item.first, item.second);
    EXPECT_EQ(s.GetCode(), minikv::STATUS_OKAY);
  }
  // step2: get all item;
  for (auto item : get_data) {
    s = db.Get(get_option, item.first, val_place_holder);
    EXPECT_EQ(s.GetCode(), minikv::STATUS_OKAY);
    EXPECT_EQ(item.second, val_place_holder);
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

TEST(database_test, put_get_loops1) {
  minikv::DatabaseOptions db_option;
  auto max_key_sz = db_option._max_key_size;
  auto max_val_sz = db_option._max_val_size;

  std::vector<std::pair<std::string, std::string>> put_data;
  std::vector<std::pair<std::string, std::string>> get_data;

  auto loops = 60000;

  // 1.
  gen_test_case(loops, 3, 3, put_data, get_data);
  put_then_get(put_data, get_data);
  // 2.
  gen_test_case(loops, 5, 5, put_data, get_data);
  put_then_get(put_data, get_data);
  // 3.
  gen_test_case(loops, 10, 10, put_data, get_data);
  put_then_get(put_data, get_data);
  // 4.
  gen_test_case(loops, 20, 20, put_data, get_data);
  put_then_get(put_data, get_data);
  // 5.
  gen_test_case(loops, 50, 50, put_data, get_data);
  put_then_get(put_data, get_data);
  // 6.
  gen_test_case(loops, 100, 100, put_data, get_data);
  put_then_get(put_data, get_data);
  // 7.
  gen_test_case(loops, max_key_sz, max_val_sz, put_data, get_data);
  put_then_get(put_data, get_data);
}

///////////////////////////////////////////////////////////
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
