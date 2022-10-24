#include <gtest/gtest.h>

#include <cstdint>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <thread>
#include <vector>

#include "../include/status.h"
#include "../include/storageengine.h"
#include "../include/utils.h"

/*
file_resource test name spec;
1、
2、
3、
4、
5、

*/

TEST(file_resource, insert_one_record_len_10) {
  minikv::DatabaseOptions db_options;
  minikv::FileResource fr(db_options);

  minikv::Record record;

  std::string key = minikv::Utils::GenRandString(10);
  std::string val = minikv::Utils::GenRandString(10);

  record._is_deleted = true;
  record._key = std::vector<char>(key.begin(), key.end());
  record._key.push_back('\0');
  record._val = std::vector<char>(val.begin(), val.end());
  record._val.push_back('\0');
  record._key_size = record._key.size();
  record._val_size = record._val.size();
  record._key_margin = db_options._max_key_size - record._key_size;
  record._val_margin = db_options._max_val_size - record._val_size;
  //   mmap ptr.
  auto ptr = fr.GetCurrentRecordPtr();
  //   record encode.
  if (!fr.IsFileFull(record.GetRecordSize())) {
    auto s = minikv::Record::EncodeRecord(ptr, record);
    if (!s.IsOK()) std::cout << s.ToString() << std::endl;
  }
  minikv::Record read_record;
  //   record decode.
  minikv::Record::DecodeRecord(ptr, read_record);
  //
  EXPECT_EQ(record._is_deleted, read_record._is_deleted);
  EXPECT_EQ(record._key_size, read_record._key_size);
  EXPECT_EQ(record._val_size, read_record._val_size);
  EXPECT_EQ(record._key_margin, read_record._key_margin);
  EXPECT_EQ(record._val_margin, read_record._val_margin);
  EXPECT_EQ(record._key, read_record._key);
  EXPECT_EQ(record._val, read_record._val);
}

TEST(file_resource, insert_one_record_len_max) {
  minikv::DatabaseOptions db_options;
  minikv::FileResource fr(db_options);

  minikv::Record record;

  std::string key = minikv::Utils::GenRandString(db_options._max_key_size - 1);
  std::string val = minikv::Utils::GenRandString(db_options._max_val_size - 1);

  record._is_deleted = true;
  record._key = std::vector<char>(key.begin(), key.end());
  record._key.push_back('\0');
  record._val = std::vector<char>(val.begin(), val.end());
  record._val.push_back('\0');
  record._key_size = record._key.size();
  record._val_size = record._val.size();
  record._key_margin = db_options._max_key_size - record._key_size;
  record._val_margin = db_options._max_val_size - record._val_size;
  // mmap ptr.
  auto ptr = fr.GetCurrentRecordPtr();
  // record encode.
  if (!fr.IsFileFull(record.GetRecordSize())) {
    auto s = minikv::Record::EncodeRecord(ptr, record);
    if (!s.IsOK()) std::cout << s.ToString() << std::endl;
  }
  minikv::Record read_record;
  // record decode.
  minikv::Record::DecodeRecord(ptr, read_record);
  // test check
  EXPECT_EQ(record._is_deleted, read_record._is_deleted);
  EXPECT_EQ(record._key_size, read_record._key_size);
  EXPECT_EQ(record._val_size, read_record._val_size);
  EXPECT_EQ(record._key_margin, read_record._key_margin);
  EXPECT_EQ(record._val_margin, read_record._val_margin);
  // examine _key
  for (uint64_t i = 0; i < record._key_size; i++) {
    EXPECT_EQ(record._key[i], read_record._key[i]);
  }
  // examine _val
  for (uint64_t i = 0; i < record._key_size; i++) {
    EXPECT_EQ(record._val[i], read_record._val[i]);
  }
}

TEST(file_resource, insert_full_record_len_10) {
  minikv::DatabaseOptions db_options;
  minikv::FileResource fr(db_options);

  minikv::Record record;
  minikv::Record read_record;

  std::string key = minikv::Utils::GenRandString(10);
  std::string val = minikv::Utils::GenRandString(10);

  record._is_deleted = true;
  record._key = std::vector<char>(key.begin(), key.end());
  record._key.push_back('\0');
  record._val = std::vector<char>(val.begin(), val.end());
  record._val.push_back('\0');
  record._key_size = record._key.size();
  record._val_size = record._val.size();
  record._key_margin = db_options._max_key_size - record._key_size;
  record._val_margin = db_options._max_val_size - record._val_size;

  int loops = db_options._max_single_file_size / record.GetRecordSize();
  printf("record.size = %d\t loops = %d\n", record.GetRecordSize(), loops);
  for (auto i = 0; i < loops; i++) {
    std::cout << "\riter = " << i + 1 << std::flush;

    auto ptr = fr.GetCurrentRecordPtr();
    minikv::Record::EncodeRecord(ptr, record);
    minikv::Record::DecodeRecord(ptr, read_record);
    EXPECT_EQ(record._is_deleted, read_record._is_deleted);
    EXPECT_EQ(record._key_size, read_record._key_size);
    EXPECT_EQ(record._val_size, read_record._val_size);
    EXPECT_EQ(record._key_margin, read_record._key_margin);
    EXPECT_EQ(record._val_margin, read_record._val_margin);
    // examine _key
    for (uint64_t i = 0; i < record._key_size; i++) {
      EXPECT_EQ(record._key[i], read_record._key[i]);
    }
    // examine _val
    for (uint64_t i = 0; i < record._key_size; i++) {
      EXPECT_EQ(record._val[i], read_record._val[i]);
    }

    // re-gen same size `_key` and `_val` and `_is_deleted`
    record._key.clear();
    record._val.clear();
    read_record._key.clear();
    read_record._val.clear();
    std::string key = minikv::Utils::GenRandString(10);
    std::string val = minikv::Utils::GenRandString(10);

    record._is_deleted = !record._is_deleted;
    record._key = std::vector<char>(key.begin(), key.end());
    record._key.push_back('\0');
    record._val = std::vector<char>(val.begin(), val.end());
    record._val.push_back('\0');
    fr.IncreaseRecordPtr(record.GetRecordSize());
  }
  std::cout << std::endl;
}

TEST(file_resource, insert_full_record_len_max) {
  minikv::DatabaseOptions db_options;
  minikv::FileResource fr(db_options);

  minikv::Record record;
  minikv::Record read_record;

  std::string key = minikv::Utils::GenRandString(db_options._max_key_size - 1);
  std::string val = minikv::Utils::GenRandString(db_options._max_key_size - 1);

  record._is_deleted = true;
  record._key = std::vector<char>(key.begin(), key.end());
  record._key.push_back('\0');
  record._val = std::vector<char>(val.begin(), val.end());
  record._val.push_back('\0');
  record._key_size = record._key.size();
  record._val_size = record._val.size();
  record._key_margin = db_options._max_key_size - record._key_size;
  record._val_margin = db_options._max_val_size - record._val_size;

  int loops = db_options._max_single_file_size / record.GetRecordSize();
  printf("record.size = %d\t loops = %d\n", record.GetRecordSize(), loops);
  for (auto i = 0; i < loops; i++) {
    std::cout << "\riter = " << i + 1 << std::flush;

    auto ptr = fr.GetCurrentRecordPtr();
    minikv::Record::EncodeRecord(ptr, record);
    minikv::Record::DecodeRecord(ptr, read_record);
    EXPECT_EQ(record._is_deleted, read_record._is_deleted);
    EXPECT_EQ(record._key_size, read_record._key_size);
    EXPECT_EQ(record._val_size, read_record._val_size);
    EXPECT_EQ(record._key_margin, read_record._key_margin);
    EXPECT_EQ(record._val_margin, read_record._val_margin);
    // examine _key
    for (uint64_t i = 0; i < record._key_size; i++) {
      EXPECT_EQ(record._key[i], read_record._key[i]);
    }
    // examine _val
    for (uint64_t i = 0; i < record._key_size; i++) {
      EXPECT_EQ(record._val[i], read_record._val[i]);
    }

    // re-gen same size `_key` and `_val` and `_is_deleted`
    record._key.clear();
    record._val.clear();
    read_record._key.clear();
    read_record._val.clear();
    std::string key =
        minikv::Utils::GenRandString(db_options._max_key_size - 1);
    std::string val =
        minikv::Utils::GenRandString(db_options._max_val_size - 1);

    record._is_deleted = !record._is_deleted;
    record._key = std::vector<char>(key.begin(), key.end());
    record._key.push_back('\0');
    record._val = std::vector<char>(val.begin(), val.end());
    record._val.push_back('\0');
    fr.IncreaseRecordPtr(record.GetRecordSize());
  }
  std::cout << std::endl;
}

TEST(file_resource, insert_different_sz_kv) {}

TEST(file_resource, insert_record_key_overflow) {}

TEST(file_resource, insert_record_val_overflow) {}

// TODO(shang): insert record number overflow
// TODO(shang): record other field violation test
// TODO: insert same key.