#ifndef INCLUDE_STORAGEENGINE_H_
#define INCLUDE_STORAGEENGINE_H_

#include <city.h>

#include <cstdint>
#include <fstream>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "./eventmanager.h"
#include "./file_resource.h"
#include "./hash.h"
#include "./mem_layout.h"
#include "./options.h"
#include "./status.h"

namespace minikv {

/*
从<hashed-kay:64, location:64>中得反向计算得到key的位置

*/

class StorageEngine {
 public:
  StorageEngine(DatabaseOptions, Event<std::vector<Record>>*);

  ~StorageEngine() {}

  Status Get(const GetOption&, const std::string&, std::string&);

  Status Delete(const PutOption&, const std::string&);

  void Stop();

 private:
  // 写数据到文件
  void buffer_store_loop();

  bool _thread_running;

  std::thread _buffer_store_handler;

  // sync with the thread in wb.
  Event<std::vector<Record>>* _sync_event;

  DatabaseOptions _db_options;

  Hash* _hash;

  bool _mmap_file_full;

  // index to buffer all record's <hashed-key, location>
  // where location refer to which file and what offset inside file.
  std::multimap<uint64_t, uint64_t> _index;

  // mmap的
  FileResource* _file_resource;
};

}  // namespace minikv

#endif  // INCLUDE_STORAGEENGINE_H_
