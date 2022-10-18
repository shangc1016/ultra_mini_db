#ifndef __WRITEBUFFER_H_
#define __WRITEBUFFER_H_

#include <array>
#include <cstdint>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>


#include "options.h"
#include "status.h"
#include "record.h"
#include "eventmanager.h"
// #include "database.h"

namespace minikv {

// 两个缓冲buffer，轮流交给storageEngine刷新到文件，这个有一个专门的线程处理
// 信号处理：停止服务
// 线程协作机制

class WriteBuffer{

public:

    WriteBuffer(DatabaseOptions, Event<std::vector<Record>>*);

    ~WriteBuffer() { Close(); }


    Status Get(GetOption&, std::string&, std::string*);

    Status Put(PutOption&, std::string&, std::string&);

    Status Delete(PutOption&, std::string&);

    void Close();


private:

    // flush线程执行的函数
    void* buffer_flush_loop();

    // 两个buffer: income buffer、flush buffer
    std::array<std::vector<Record>, 2> _buffers;
    // 两个buffer的size
    std::array<uint64_t, 2> _buffer_size;
    
    // 写income_buffer的锁
    std::mutex _write_income_lock;

    // 交换income_buffer和flush_buffer的锁
    std::mutex _swap_index_lock;

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
    std::thread     _buffer_flush_handler;

    Event<std::vector<Record>> *_sync_event;


    DatabaseOptions _db_options;

};





}

#endif