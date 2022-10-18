#ifndef __EVENT_MANAGER_H_
#define __EVENT_MANAGER_H_

#include <condition_variable>
#include <mutex>

namespace minikv {

// 模板类，事件通知
// 用于两个线程之间同步,
// 有两个cv
template<typename T>
class Event{


public:


    Event<T>(){
        _ready = false;
        _done = false;
    }


    ~Event() {}

    
    // a线程发送数据，并且阻塞，等待处理完成
    void SendAndWaitForDone(T data){
    std::unique_lock<std::mutex> _ready_lock(_ready_mutex);
    _data = data;
    _have_data = true;
    _ready_mutex.unlock();

    _ready_cv.notify_one();
    
    std::unique_lock<std::mutex> _done_lock(_done_mutex);
    _done_cv.wait(_done_lock, [this]{ return !_have_data;});
    _done_mutex.unlock();

}

    // b线程等待数据到来，并且处理
    T Wait(){
    std::unique_lock<std::mutex> _ready_lock(_ready_mutex);
    _ready_cv.wait(_ready_lock, [this]{ return _have_data; });
    _ready_mutex.unlock();
    return _data;
}

    // b线程完成任务，通知a线程
    void Done(){
    std::unique_lock<std::mutex> _done_lock(_done_mutex);
    _have_data = false;
    _done_cv.notify_one();
    _done_mutex.unlock();
}



private:
    T _data;
    bool _have_data;
    std::mutex _ready_mutex;
    std::mutex _done_mutex;
    std::condition_variable _ready_cv;
    std::condition_variable _done_cv;
    bool _ready;
    bool _done;

};



}



#endif