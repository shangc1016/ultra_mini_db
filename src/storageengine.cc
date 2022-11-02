#include "../include/storageengine.h"

#include <city.h>
#include <fcntl.h>
#include <inttypes.h>
#include <sys/mman.h>
#include <sys/stat.h>
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

  _mmap_file_full = false;
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
      // step0: judge whether been deleted.
      if (item._is_deleted) continue;
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
      auto record_size = Record::RecordSize(item);
      auto current_ptr = _file_resource->GetCurrentRecordPtr();
      if (!_file_resource->IsFileFull(record_size + current_ptr)) {
        Record::EncodeRecord(current_ptr, item);
        _file_resource->IncreaseRecordPtr(record_size);
        // printf("mmap file full, FIXME==\n");
      } else {
        // for first edition, we just encode header to current file start pos,
        // and still keep `_index` in mem, `_index` only flush to one specific
        // index file when db shutdown.

        // step1: fill header;
        auto index_item_nums = _index.size();
        minikv::RecordHeader record_header(
            0, _db_options._internal_hstable_header_sz, sizeof(uint64_t) * 2,
            index_item_nums);
        // step2: encode header;
        auto start_ptr = _file_resource->GetMmapStartPtr();
        minikv::RecordHeader::EncodeRecordHeader(start_ptr, record_header);
        // step3: file_rsource change to another file.
        _file_resource->ChangeToNewFile();
        fprintf(stdout, "change to new file\n");

        // step4: set flag, and break;
      }
    }
    _sync_event->Done();
    if (_mmap_file_full) exit(0);
  }
}

// TODO: at this time, not use get_option.
Status StorageEngine::Get(const GetOption&, const std::string& key,
                          std::string& value) {
  if (_index.size() == 0)
    return Status(STATUS_NOT_FOUND, "[SE:GET]: _index empty.");
  // TODO: FIXME
  if (_mmap_file_full) exit(0);
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
    fprintf(stdout, "[SE:GET]: `_index` not found.\n");
    return Status(STATUS_NOT_FOUND, "[SE:GET]:");
  }
  // step3: decode from mmap address to object minikv::Record, and get it's val.
  // is it possible to decrease overhead of `Get` here, cause here we don't care
  // much other field of record.

  range.second--;
  for (auto iter = range.second; iter != _index.end(); --iter) {
    auto foffset = Utils::LocationDecodeFileOffset(iter->second);
    auto fnumber = Utils::LocationDecodeFileNum(iter->second);
    // TODO: fnumber not equal to 1, means need to decode from another
    // file.
    if (fnumber != 1) {
      fprintf(stdout, "fnumber = %d\n", fnumber);
      exit(0);
    }
    if (foffset > _file_resource->GetCurrentRecordPtr())
      return Status(STATUS_PTR_EXCEPTION, "[SE:GET]: file_offset exceed.");

    Record tmp;
    Record::DecodeRecord(foffset, tmp);
    if (strncmp(tmp._key.data(), key.data(), tmp._key_size - 1)) break;
    if (tmp._is_deleted) continue;
    // cause tmp.val have terminal '\0', so here we use .data();
    value = std::string(tmp._val.data());
    return Status(STATUS_OKAY, "[SE:GET]: ok.");
  }

  return Status(STATUS_NOT_FOUND, "[SE:GET]: not found.");
}

Status StorageEngine::Delete(const PutOption&, const std::string& key) {
  // almost same as `Get` in `se`.

  if (_mmap_file_full) exit(0);

  // step1: get hashed_key.
  auto _key = std::vector<char>(key.begin(), key.end());
  _key.push_back('\0');
  auto hashed_key = _hash->HashFunction(_key.data(), _key.size());
  // step2: search `hashed_key` in `_index` to find same hashed_key items.
  auto range = _index.equal_range(hashed_key);

  if (range.first == range.second) {
    fprintf(stdout, "StorageEngine::Delete: not found.\n");
    return Status(STATUS_NOT_FOUND, "StorageEngine::Delete not found.");
  }
  // we should just delete latest/newest insert item, so here we should
  // reverse traverse `range`, but no such function call, so we change every
  // item we net when traverse.

  range.second--;
  for (auto iter = range.second; iter != _index.end(); --iter) {
    auto foffset = Utils::LocationDecodeFileOffset(iter->second);
    auto fnumber = Utils::LocationDecodeFileNum(iter->second);
    if (fnumber != 1) {
      fprintf(stdout, "fnumber = %d\n", fnumber);
      exit(0);
    }
    if (foffset > _file_resource->GetCurrentRecordPtr())
      return Status(STATUS_PTR_EXCEPTION, "[SE:DELETE]: file_offset exceed.");
    Record tmp;
    // TODO: frequently decode and encode whold record, thus frequently meecpy,
    // cause slow down, try to just encode & decode just specific attr in
    // `minikv::Record`;
    Record::DecodeRecord(foffset, tmp);
    if (strncmp(tmp._key.data(), key.data(), tmp._key_size - 1)) break;
    if (tmp._is_deleted) continue;
    tmp._is_deleted = true;
    Record::EncodeRecord(foffset, tmp);
    return Status(STATUS_OKAY, "[SE:DELETE]: ok.");
  }
  return Status(STATUS_NOT_FOUND, "[SE:DELETE]: not found.");
}

}  // namespace minikv