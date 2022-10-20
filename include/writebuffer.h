#ifndef __WRITE_BUFFER_H_
#define __WRITE_BUFFER_H_

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

    ~WriteBuffer();

    void Stop();



    Status Get(GetOption&, std::string&, std::string*);

    Status Put(PutOption&, std::string&, std::string&);

    Status Delete(PutOption&, std::string&);



private:

    // flush线程执行的函数
    void buffer_flush_loop();

    // 两个buffer: income buffer、flush buffer
    std::array<std::vector<Record>, 2> _buffers;
 
    
    std::mutex _lock_get_level1;

    std::mutex _lock_put_level2;

    std::mutex _lock_swap_index3;
    // 在线程把flush_buffer数据交给se之前，flush_buffer是可以查询的
    std::mutex _lock_flush_index4;


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

    // 这个eventmanager用于wb thread和se threa的同步.
    Event<std::vector<Record>> *_sync_event;

    // 还需要主线程和wb thread之间的同步.
    Event<int> _put_swap_sync;



    DatabaseOptions _db_options;

};





}

#endif