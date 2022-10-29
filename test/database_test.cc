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

// generated kv array.
typedef std::vector<std::pair<std::string, std::string>> DataVec;

// overflow test.
void overflow_test(int key_size, int val_size, minikv::StatusCode code) {
  minikv::DatabaseOptions db_opt;
  minikv::Database db(db_opt, "/tmp/minikv");

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
void gen_test_case(int loops, int key_size, int val_size, DataVec &put_data,
                   DataVec &get_data) {
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
void put_then_get(DataVec &put_data, DataVec &get_data) {
  minikv::DatabaseOptions db_opt;
  minikv::Database db(db_opt, "/tmp/minikv");

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

// delete test entrance.
void delete_test(DataVec &put_data, DataVec &get_data) {
  minikv::DatabaseOptions db_opt;
  minikv::Database db(db_opt, "/tmp/minikv");

  std::string val_place_holder;

  minikv::PutOption put_option;
  minikv::GetOption get_option;

  minikv::Status s;
  // step1: put all item in `put_data`;
  for (auto item : put_data) {
    s = db.Put(put_option, item.first, item.second);
    EXPECT_EQ(s.GetCode(), minikv::STATUS_OKAY);
  }
  // step2: delete all item in `get_data`;
  for (auto item : get_data) {
    s = db.Delete(put_option, item.first);
    EXPECT_EQ(s.GetCode(), minikv::STATUS_OKAY);
  }
  // step3: get all item in `get_data`;
  for (auto item : get_data) {
    s = db.Get(get_option, item.first, val_place_holder);
    EXPECT_EQ(s.GetCode(), minikv::STATUS_NOT_FOUND);
  }
}

////////////////////== TEST ==////////////////////
TEST(database_test, test_key_overflow) {
  minikv::DatabaseOptions db_opt;
  overflow_test(db_opt._max_key_size + 1, db_opt._max_val_size,
                minikv::STATUS_KEY_TOO_LONG);
}

TEST(database_test, test_val_overflow) {
  minikv::DatabaseOptions db_opt;
  overflow_test(db_opt._max_key_size, db_opt._max_val_size + 1,
                minikv::STATUS_VAL_TOO_LONG);
}

TEST(database_test, test_k_v_overflow) {
  minikv::DatabaseOptions db_opt;
  overflow_test(db_opt._max_key_size + 1, db_opt._max_val_size + 1,
                minikv::STATUS_KEY_TOO_LONG);
}

TEST(database_test, put_get_loops1) {
  minikv::DatabaseOptions db_opt;
  auto max_key_sz = db_opt._max_key_size;
  auto max_val_sz = db_opt._max_val_size;

  DataVec put_data;
  DataVec get_data;

  auto loops = 80000;

  // 1.
  gen_test_case(loops, 1, 1, put_data, get_data);
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

TEST(database_test, test_delete) {
  DataVec put_data;
  DataVec get_data;

  auto loops = 6000;

  gen_test_case(loops, 1, 1, put_data, get_data);

  delete_test(put_data, get_data);
}

TEST(database_test, str_vec_test) {
  std::string str = "shang";

  std::vector<char> vec = std::vector<char>(str.begin(), str.end());

  vec.push_back('\0');

  fprintf(stdout, "vec size = %ld\n", vec.size());

  fprintf(stdout, "str size = %ld\n", str.length());
}
////////////////////== TEST END ==////////////////////
