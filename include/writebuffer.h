#ifndef INCLUDE_WRITEBUFFER_H_
#define INCLUDE_WRITEBUFFER_H_

#include <array>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "./eventmanager.h"
#include "./options.h"
#include "./record.h"
#include "./status.h"
// #include "database.h"

namespace minikv {

// 两个缓冲buffer，轮流交给storageEngine刷新到文件，这个有一个专门的线程处理
// 信号处理：停止服务
// 线程协作机制

class WriteBuffer {
 public:
  WriteBuffer(DatabaseOptions, Event<std::vector<Record>> *);

  ~WriteBuffer() {}

  void Stop();

  Status Get(GetOption &, const std::string &, std::string *);

  Status Put(PutOption &, const std::string &, const std::string &);

  Status Delete(PutOption &, const std::string &);

 private:
  // flush线程执行的函数
  void buffer_flush_loop();

  // 两个buffer: income buffer、flush buffer
  std::array<std::vector<Record>, 2> _buffers;

  std::mutex _lock_get_put_level1;

  std::mutex _lock_swap_index2;

  // 到那时查找、删除的时候，有可能会使用flush_buffer
  // 如果正在delete、get的时候，正在put数据，或者正在swap
  // 那遍历会不会失效？
  // 需要额外的锁来保证数据安全
  // get、delete的时候不能swap，

  bool _thread_running;

  // 两个buffer的下标
  int _income_index;
  int _flush_index;
  // 处理flush buffer的线程
  std::thread _buffer_flush_handler;

  // 这个eventmanager用于wb thread和se threa的同步.
  Event<std::vector<Record>> *_sync_event;

  // 还需要主线程和wb thread之间的同步.
  Event<int> _put_swap_sync;

  DatabaseOptions _db_options;
};

}  // namespace minikv

#endif  // INCLUDE_WRITEBUFFER_H_
