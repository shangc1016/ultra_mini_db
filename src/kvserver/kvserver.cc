#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iterator>
#include <string>
#include <vector>

#include "../../include/database.h"
#include "../../include/file.h"
#include "../../include/log.h"
#include "../../include/record.h"
#include "../../include/utils.h"

int main() {
  minikv::DatabaseOptions database_option;
  minikv::FileResource file_resource(database_option);
  auto ptr = file_resource.GetCurrentRecordPtr();

  std::string key =
      "1234567890|1234567890|1234567890|1234567890|1234567890|1234567890|"
      "1234567890|1234567890|1234567890|1234567890|1234567890|1234567890|";
  std::string val =
      "0123456789|0123456789|0123456789|0123456789|0123456789|0123456789|"
      "0123456789|0123456789|0123456789|0123456789|0123456789|0123456789|";

  minikv::Record record1;

  record1._is_deleted = true;
  record1._key = std::vector<char>(key.begin(), key.end());
  record1._val = std::vector<char>(val.begin(), val.end());
  record1._key.push_back('\0');
  record1._val.push_back('\0');

  record1._key_size = record1._key.size();
  record1._val_size = record1._val.size();
  // printf("key size = %ld, cstr size = %ld\n", key.size(),
  // record1._key.size());
  record1._key_margin = database_option._max_key_size - record1._key_size;
  record1._val_margin = database_option._max_val_size - record1._val_size;

  //

  if (!file_resource.IsFileFull(record1.GetRecordSize())) {
    record1.EncodeRecord(ptr);
    file_resource.IncreaseRecordPtr(record1.GetRecordSize());
  }

  minikv::Record record2;

  minikv::Record::DecodeRecord(ptr, record2);
  printf("========");
  printf("id_deleted = %d\n", record2._is_deleted);
  printf("_key_size = %ld\n", record2._key_size);
  printf("_val_size = %ld\n", record2._val_size);
  printf("_key_margin = %ld\n", record2._key_margin);
  printf("_key = %s\n", record2._key.data());
  printf("_val = %s\n", record2._val.data());
}
