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
void loops_size_test(int loops, int key_size, int val_size) {
  minikv::DatabaseOptions database_option;
  minikv::Database db(database_option, "/tmp/minikv");

  std::string key;
  std::string val;
  std::string val_place_holder;

  minikv::PutOption put_option;
  minikv::GetOption get_option;

  minikv::Status s;

  for (auto i = 0; i < loops; i++) {
    if (loops > 1000) std::cout << "\riter = " << i + 1 << std::flush;
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

void put_then_get_test(int loops) {
  minikv::DatabaseOptions database_option;
  minikv::Database db(database_option, "/tmp/minikv");

  std::vector<std::pair<std::string, std::string>> vec;

  std::string key;
  std::string val;
  std::string val_place_holder;

  minikv::PutOption put_option;
  minikv::GetOption get_option;

  minikv::Status s;

  for (auto i = 0; i < loops; i++) {
    key = minikv::Utils::GenRandString(database_option._max_key_size / 2);
    val = minikv::Utils::GenRandString(database_option._max_val_size / 2);
    // temp store.
    vec.push_back(std::pair<std::string, std::string>(key, val));

    s = db.Put(put_option, key, val);
    EXPECT_EQ(s.GetCode(), minikv::STATUS_OKAY);
    key.clear();
    val.clear();
  }

  for (auto item : vec) {
    s = db.Get(get_option, item.first, &val_place_holder);
    EXPECT_EQ(s.GetCode(), minikv::STATUS_OKAY);
    EXPECT_EQ(item.second, val_place_holder);
    val_place_holder.clear();
  }
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

/*
`test_name` name spec:
1、`test_loops<loops>_key<key_length>_val<val_length>`
- key_length\val_length can also be `max`.

2、`test_key_overflow`

3、`test_val_overflow`

4、`test_k_v_overflow`

*/

TEST(database_test, test_loops1_key10_val10) { loops_size_test(1, 10, 10); }

TEST(database_test, test_loops1_keymax_valmax) {
  minikv::DatabaseOptions db_option;
  loops_size_test(1, db_option._max_key_size, db_option._max_val_size);
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

/*

put all, then get all test.

`test_name` name spec:

1、test_put<nums>_get<nums>
2、
3、
4、
5、

*/

TEST(database_test, test_put_then_get_len_less) {
  // note to consider with db_option._max_wb_buffer_size
  minikv::DatabaseOptions db_option;
  put_then_get_test(db_option._max_wb_buffer_size - 1);
}

// FIXME: not pass.
TEST(database_test, test_put_then_get_len_eq_wb_len) {
  // note to consider with db_option._max_wb_buffer_size
  minikv::DatabaseOptions db_option;

  put_then_get_test(db_option._max_wb_buffer_size - 1);
}

// TEST(database_test, test_put_then_get_len_exceed_wb_len) {
//   // note to consider with db_option._max_wb_buffer_size
//   minikv::DatabaseOptions db_option;

//   put_then_get_test(db_option._max_wb_buffer_size + 1);
// }

/*
TODO:
1、persist storage test: open db, put then close db,. reopen db and get.
2、get test: get data that should be persist on disk.
3、get test: get kv when system is in state of `swap`.
4、get test: get kv that should store on `wb`.
5、get test: get kv that should store on `se`.

*/