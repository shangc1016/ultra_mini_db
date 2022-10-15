#ifndef __WRITEBUFFER_H_
#define __WRITEBUFFER_H_

#include <array>
#include <cstdint>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>


#include "options.h"
#include "status.h"

namespace minikv {

// 两个缓冲buffer，轮流交给storageEngine刷新到文件，这个有一个专门的线程处理
// 信号处理：停止服务
// 线程协作机制

class WriteBuffer{

public:

    WriteBuffer();

    ~WriteBuffer() { Close(); }


    Status Get(GetOption&, std::string&, std::string*);

    Status Put(PutOption&, std::string&, std::string&);

    Status Delete(PutOption&, std::string&);

    void Close();


private:

    // 线程执行的函数
    void buffer_flush_loop();

    // 两个buffer: income buffer、flush buffer
    std::array<std::vector<std::pair<std::string, std::string>>, 2> _buffers;
    // 两个buffer的size
    std::array<int, 2> _size;
    // 写buffer的锁
    std::mutex _write_lock;
    // buffer达到这个大小就需要swap，然后flush了
    uint64_t _buffer_size;

    // 两个buffer的下标
    int _income_index;
    int _flush_index;
    // 处理flush buffer的线程
    std::thread buffer_flush_handler;

};





}

#endif