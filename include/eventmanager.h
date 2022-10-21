#ifndef INCLUDE_EVENTMANAGER_H_
#define INCLUDE_EVENTMANAGER_H_

#include <condition_variable>
#include <mutex>

namespace minikv {

// 模板类，事件通知
// 用于两个线程之间同步,
// 有两个cv
template <typename T>
class Event {
 public:
  Event<T>() { has_data = false; }
  ~Event() {}

  // a线程发送数据，并且阻塞，等待处理完成
  void SendAndWaitForDone(T data) {
    std::unique_lock<std::mutex> _start_lock(_unique_mutex);
    std::unique_lock<std::mutex> _lock(_mutex);

    _data = data;
    has_data = true;

    _ready_cv.notify_one();
    _done_cv.wait(_lock);
  }

  // b线程等待数据到来，并且处理
  T Wait() {
    std::unique_lock<std::mutex> _lock(_mutex);
    _ready_cv.wait(_lock, [this] { return has_data; });
    return _data;
  }

  // b线程完成任务，通知a线程
  void Done() {
    std::unique_lock<std::mutex> _lock(_mutex);
    has_data = false;
    // _done_lock.unlock();
    _done_cv.notify_one();
  }

  void NotifyWait() {
    std::unique_lock<std::mutex> _lock(_mutex);
    _ready_cv.notify_one();
  }

 private:
  T _data;
  bool has_data;
  std::mutex _mutex;
  std::mutex _unique_mutex;
  std::condition_variable _ready_cv;
  std::condition_variable _done_cv;
  bool _ready;
  bool _done;
};

}  // namespace minikv

#endif  // INCLUDE_EVENTMANAGER_H_
