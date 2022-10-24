#include "../include/storageengine.h"

#include <city.h>
#include <fcntl.h>
#include <inttypes.h>
#include <sys/types.h>
#include <unistd.h>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <ios>
#include <iostream>
#include <ratio>
#include <thread>
#include <utility>
#include <vector>

#include "../include/utils.h"

namespace minikv {

StorageEngine::StorageEngine(DatabaseOptions db_option,
                             Event<std::vector<Record>>* event) {
  _db_options = db_option;
  _sync_event = event;

  // flag to start thread, and stop it.
  _thread_running = true;
  // pass a class method to std::thread, add class object this.
  _buffer_store_handler = std::thread(&StorageEngine::buffer_store_loop, this);

  if (_buffer_store_handler.joinable()) {
    _buffer_store_handler.detach();
  }

  // 文件资源，用来持久化kv数据，使用mmap的方式
  _file_resource = new FileResource(db_option);

  // hash function switch
  switch (_db_options._hash_type) {
    case MurmurHash128:
      _hash = new MurmurHash();
      break;
    case CityHash64:
    default:
      _hash = new CityHash();
  }
}

void StorageEngine::Stop() { _thread_running = false; }

// step1: hash it,
// step2: write it to mmaped file address space, and get the location where it
// exist.
// step3: for each item in `buffered_data`, get coupled data -> <hashed-key,
// location> where location contain info about `which file` and `what offset
// within the file`. the list we get is called `index` next step: considerate
// how and where to persist those `index`.
void StorageEngine::buffer_store_loop() {
  // infinite loop, to handle data from `wb`.
  while (true) {
    if (_thread_running == false) break;

    auto buffer = _sync_event->Wait();

    for (auto item : buffer) {
      // step 1: get hashed-key.
      auto hashed_key = _hash->HashFunction(item._key.data(), item._key_size);
      // step 2: add record info to index. including hashed-key and
      // location here we use filenumber and offset inside file to
      // represent location.
      auto file_number = _file_resource->GetCurrentFileNumber();
      auto file_offset = _file_resource->GetCurrentRecordPtr();
      // BUG: notice that the type of `file_offset` is uint64_t, and the
      // type of `file_number` is uint32_t, and `GenLocation` just
      // ignore high 32 bit of file_offset, it is totally an fatal
      // error.
      auto location = Utils::GenLocation(file_number, file_offset);
      // step 3: push <hashed-key, location> to  _index;
      _index.insert(std::pair<uint64_t, uint64_t>(hashed_key, location));
      // step 4: memcpy record header to mmap-ed address.
      auto record_size = item.GetRecordSize();
      if (!_file_resource->IsFileFull(record_size)) {
        Record::EncodeRecord(_file_resource->GetCurrentRecordPtr(), item);
        _file_resource->IncreaseRecordPtr(record_size);
      } else {
        // TODO: if mmap buffer full, some steps should be done.
        // step1:
        fprintf(stdout, "mmap file full, FIXME\n");
      }
    }

    _sync_event->Done();
  }
}
// TODO: at this time, not use get_option.
Status StorageEngine::Get(GetOption&, const std::string& key,
                          std::string* value) {
  // step1: same as persist to disk, calculate hashed-key.
  auto _key = std::vector<char>(key.begin(), key.end());
  _key.push_back('\0');
  auto hashed_key = _hash->HashFunction(_key.data(), _key.size());

  // step2: search multimap `index` by hashed_key, here we should traverse
  // `_index` in reverse order since flush_buffer's order is put order, thus
  // time order. we just care about newest insert recoed, which is the last.
  auto range = _index.equal_range(hashed_key);
  if (range.first == range.second) {
    // not same hashed_key found in `_index`.
    fprintf(stdout, "`_index` not found\n");
    return Status(STATUS_NOT_FOUND, "StorageEngine::Get");
  }
  // step3: decode from mmap address to object minikv::Record, and get it's val.
  // is it possible to decrease overhead of `Get` here, cause here we don't care
  // much other field of record.
  // FIXME:!!!!! reverse order traverse to get newest record.
  for (auto iter = range.first; iter != range.second; ++iter) {
    if (iter->first == hashed_key) {
      auto file_offset = Utils::LocationDecodeFileOffset(iter->second);
      Record tmp_record;
      Record::DecodeRecord(file_offset, tmp_record);
      // BUG=FIXME: so ugly.
      tmp_record._val.pop_back();
      *value = std::string(tmp_record._val.begin(), tmp_record._val.end());
      return Status(STATUS_OKAY, "StorageEngine::Get OK.");
    }
  }
  return Status(STATUS_NOT_FOUND, "StorageEngine::Get");
}
}  // namespace minikv