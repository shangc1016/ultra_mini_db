#ifndef INCLUDE_STORAGEENGINE_H_
#define INCLUDE_STORAGEENGINE_H_

#include <city.h>

#include <cstdint>
#include <fstream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "./eventmanager.h"
#include "./file.h"
#include "./hash.h"
#include "./options.h"
#include "./record.h"
#include "./status.h"

namespace minikv {

/*
从<hashed-kay:64, location:64>中得反向计算得到key的位置

*/

class StorageEngine {
 public:
  StorageEngine(DatabaseOptions, Event<std::vector<Record>>*);

  ~StorageEngine() {}

  Status Get();

  void Stop();

 private:
  // 写数据到文件
  void* buffer_store_loop();

  bool _thread_running;

  std::thread _buffer_store_handler;

  // sync with the thread in wb.
  Event<std::vector<Record>>* _sync_event;

  DatabaseOptions _db_options;

  Hash* _hash;

  // mmap的
  FileResource* _file_resource;
};

}  // namespace minikv

#endif  // INCLUDE_STORAGEENGINE_H_
