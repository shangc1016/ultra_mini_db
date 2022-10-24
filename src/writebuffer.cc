#include "../include/writebuffer.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <chrono>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <exception>
#include <iostream>
#include <memory>
#include <mutex>
#include <system_error>
#include <thread>
#include <utility>
#include <vector>

#include "../include/log.h"

namespace minikv {

WriteBuffer::WriteBuffer(DatabaseOptions db_options,
                         Event<std::vector<Record>> *event) {
  _income_index = 0;
  _flush_index = 1;

  _db_options = db_options;

  _sync_event = event;

  _thread_running = true;

  _buffer_flush_handler = std::thread(&WriteBuffer::buffer_flush_loop, this);
  if (_buffer_flush_handler.joinable()) {
    _buffer_flush_handler.detach();
  }
}

void WriteBuffer::Stop() { _thread_running = false; }

// put ：写入 income_buffer
// loop: 使用 flush_buffer

// 所以swap有两个情况：
// 1. income_buffer 满了
// 2. flush_buffer  空了
// 这样的话，也需要记录每个buffer的长度
//   需要有锁保证两者的互斥
//
//

// writeBuffer thread, to deliver buffer[_flush_index] to se.
void WriteBuffer::buffer_flush_loop() {
  while (true) {
    if (_thread_running == false) break;
    // wait for signal from `Put`, indicate buffer full.
    _put_swap_sync.Wait();
    {
      // acquire all lock.
      std::lock_guard<std::mutex> lock1(_lock_get_put_level1);
      std::lock_guard<std::mutex> lock2(_lock_swap_index2);
      std::swap(_income_index, _flush_index);
      // TODO: [swap]: key point, remember to log.
    }

    _sync_event->SendAndWaitForDone(_buffers[_flush_index]);

    // `se` have done it's work, just clear buffer.
    // FIXME: it seems that chear here lead to get bug.
    // _buffers[_flush_index].clear();
    // _buffers[_flush_index].shrink_to_fit();

    // _sync_event->Reset(), signal back to `Put`.
    _put_swap_sync.Done();
  }
}

Status WriteBuffer::Put(PutOption &, const std::string &key,
                        const std::string &val) {
  minikv::Logger::Trace("WriteBuffer::Put", "");

  // key & value length validation
  if (key.size() - 1 >= _db_options._max_key_size)
    return Status(STATUS_KEY_TOO_LONG, "WriteBuffer::Put");
  if (val.size() - 1 >= _db_options._max_val_size)
    return Status(STATUS_VAL_TOO_LONG, "WriteBuffer::Put");

  // format K-V to record.
  Record record;
  record._is_deleted = false;
  record._key = std::vector<char>(key.begin(), key.end());
  record._key.push_back('\0');

  record._val = std::vector<char>(val.begin(), val.end());
  record._val.push_back('\0');
  // size include '\0'.
  fprintf(stdout, "key = %s\t", record._key.data());
  fprintf(stdout, "val = %s\n", record._val.data());
  record._key_size = record._key.size();
  record._val_size = record._val.size();
  // margin size.
  record._key_margin = _db_options._max_key_size - record._key_size;
  record._val_margin = _db_options._max_val_size - record._val_size;

  std::size_t buffer_len;
  {
    // acquire `get` and `put` lock.
    std::lock_guard<std::mutex> lock1(_lock_get_put_level1);
    _buffers[_income_index].push_back(record);
    buffer_len = _buffers[_income_index].size();
  }

  // check whether buffer full, thus to `swap`.
  //
  // current design is to use event manager to sync between Put and Loop.
  // so put and swap is sync, not async.
  // First, when put 101 records, it swap at 101, then loop flush it
  // to disk and notify Put thread, then clear flush_buffer, and back.
  // Then, Get get 101 record, can not find in income_buffer, and
  // turn to flush_buffer. at this time, put already clear flush buffer.
  // so `Get` need to search in `se` index and turn to disk...
  //
  if (buffer_len > _db_options._max_wb_buffer_size) {
    _put_swap_sync.SendAndWaitForDone(1);  // 1 here is none-sense
    _buffers[_flush_index].clear();
    _buffers[_flush_index].shrink_to_fit();
  }

  return Status(STATUS_OKAY, "WriteBuffer::Put OK.");
}  // namespace minikv

Status WriteBuffer::Get(GetOption &, const std::string &key,
                        std::string *value) {
  auto buffer = _buffers[_income_index];
  std::size_t buffer_len;
  {
    std::lock_guard<std::mutex> lock(_lock_get_put_level1);
    buffer_len = buffer.size();
  }
  // lock free.
  for (std::size_t i = 0; i < buffer_len; ++i) {
    if (strncmp(buffer[i]._key.data(), key.c_str(), buffer[i]._key_size - 1) ==
        0) {
      *value = std::string(buffer[i]._val.data());
      return Status(STATUS_OKAY, "WriteBuffer::Get::income");
    }
  }
  buffer = _buffers[_flush_index];
  {
    std::lock_guard<std::mutex> lock(_lock_get_put_level1);
    buffer_len = buffer.size();
  }
  // if `put` reach wb max size, then it will fluhs to se, and clear
  // flush_buffer. so it's none-sense to search in flush_buffer.
  //
  // fprintf(stdout, "WriteBuffer::Get, flush_buffer size = %ld\n", buffer_len);
  // // lock free.
  // for (std::size_t i = 0; i < buffer_len; ++i) {
  //   if (strncmp(buffer[i]._key.data(), key.c_str(), buffer[i]._key_size) ==
  //   0) {
  //     *value = std::string(buffer[i]._val.data());
  //     return Status(STATUS_OKAY, "WriteBuffer::Get::flush");
  //   }
  // }

  // TODO(shang): searching in `storage_engine`

  return Status(STATUS_NOT_FOUND, "writeBuffer::Get not found");
}

Status WriteBuffer::Delete(PutOption &, const std::string &key) {
  // 首先和Get一样，先income，再flush，遍历两个buffer。找到的话，修改_deleted参数
  // step1:先在flush  buffer中查找
  _lock_get_put_level1.lock();
  auto buffer = _buffers[_income_index];
  int buffer_len = buffer.size();
  _lock_get_put_level1.unlock();
  for (int i = 0; i < buffer_len; i++) {
    if (strncmp(buffer[i]._key.data(), key.c_str(), buffer[i]._key.size())) {
      _lock_get_put_level1.lock();
      // _lock_put_level2.lock();
      buffer[i]._is_deleted = true;
      // _lock_put_level2.unlock();
      _lock_get_put_level1.unlock();
      return Status(STATUS_OKAY, "WriteBuffer::Delete::income OK.");
    }
  }
  // step2: 在income buffer侦破哪个查找
  _lock_get_put_level1.lock();
  buffer = _buffers[_flush_index];
  buffer_len = buffer.size();
  _lock_get_put_level1.unlock();
  for (int i = 0; i < buffer_len; i++) {
    if (strncmp(buffer[i]._key.data(), key.c_str(), buffer[i]._key.size())) {
      _lock_get_put_level1.lock();
      // _lock_put_level2.lock();
      buffer[i]._is_deleted = true;
      // _lock_put_level2.unlock();
      _lock_get_put_level1.unlock();
      return Status(STATUS_OKAY, "WriteBuffer::Delete::income OK.");
    }
  }

  // TODO(shang): 在storageEngine中删除一个记录

  return Status(STATUS_OKAY, "WriteBuffer::Delete OK.");
}

}  // namespace minikv