#include "../include/writebuffer.h"
#include "../include/log.h"

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <exception>
#include <fcntl.h>
#include <iostream>
#include <memory>
#include <mutex>
#include <sys/types.h>
#include <sys/stat.h>
#include <system_error>
#include <thread>
#include <unistd.h>
#include <utility>


namespace minikv {

WriteBuffer::WriteBuffer(DatabaseOptions db_options, Event<std::vector<Record>> *event){

    _income_index = 0;
    _flush_index  = 1;

    _db_options = db_options;

    _sync_event = event;

    _thread_running = true;

    _buffer_flush_handler = std::thread(&WriteBuffer::buffer_flush_loop, this);
    if(_buffer_flush_handler.joinable()){
        _buffer_flush_handler.detach();
    }

}

WriteBuffer::~WriteBuffer(){

    // 退出线程
    // if(_buffer_flush_handler.joinable()){
    //     _buffer_flush_handler.join();
    // }

}

void WriteBuffer::Stop(){
    _thread_running = false;
}





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
void WriteBuffer::buffer_flush_loop(){

    while(true){


        if(_thread_running == false) break;

        // 等待Put满了，交换buffer
        _put_swap_sync.Wait();
        std::swap(_income_index, _flush_index);
        printf("wb thread loop.\n");

        _sync_event->SendAndWaitForDone(_buffers[_flush_index]);
        printf("return writeBuffer::buffer_flush_loop\n");

        // 这个buffer到最后再clear，
        _buffers[_flush_index].clear();

        // _sync_event->Reset();
        _put_swap_sync.Done();

        // swap 和 Get的时候读取flush_buffer是互斥的

        
    }
    // TODO - is thread exit successfully?
    // TODO - handle signal
}


Status WriteBuffer::Put(PutOption&, std::string &key, std::string &value){

    minikv::Logger::Trace("WriteBuffer::Put", "");


    if(key.size() > 20 || value.size() > 20){
        return Status(STATUS_STR_TOO_LONG, "WriteBuffer::Put");
    }

    Record record;

    key.copy(record._key, key.size());
    value.copy(record._value, value.size());

    record._key[key.size()] = '\0';
    record._value[value.size()] = '\0';


    record._is_deleted = false;
    record._key_size = key.size();
    record._value_size = value.size();

    // _lock_get_level1.lock();
    // _lock_put_level2.lock();
    
    _buffers[_income_index].push_back(record);


    // 默认在插入数据之后，检查income的长度z
    if(_buffers[_income_index].size() >= _db_options._max_wb_buffer_size) {
        printf("[Put]: income size = %ld\n", _buffers[_income_index].size());
        _put_swap_sync.SendAndWaitForDone(1);
        printf("[Put]: =========== swaped\n");
    }

    // _lock_put_level2.unlock();
    // _lock_get_level1.unlock();



    // 插入了一条记录，然后马上get查询，结果not found


    return Status(STATUS_OKAY, "WriteBuffer::Put OK.");
}


Status WriteBuffer::Get(GetOption &, std::string &key, std::string *value){
    
    {   // search in income_buffer.
        std::lock_guard<std::mutex> lock(_lock_get_level1);

        auto buffer = _buffers[_income_index];
        int buffer_len = buffer.size();

        for(int i = 0; i < buffer_len; ++i){
            // 逐个遍历，查找
            if(strncmp(buffer[i]._key, key.c_str(), strlen(buffer[i]._key)) == 0){
                *value = std::string(buffer[i]._value);
                return Status(STATUS_OKAY, "WriteBuffer::Get::income");
            }
        }
    }
    
    {   // search in flush_buffer.
    // 这儿用的是flush_buffer的引用，所以可以不用管se怎么处理buffer
        std::lock_guard<std::mutex> lock(_lock_get_level1);

        auto& buffer = _buffers[_flush_index];

        for(auto item : buffer){
            if(strncmp(item._key, key.c_str(), strlen(item._key)) == 0){
                *value = std::string(item._value);
                return Status(STATUS_OKAY, "WriteBuffer::Get::flush");
            }
        }
    }
    
    // TODO-01: 需要实现去storageEngine中查找.


    return Status(STATUS_NOT_FOUND, "writeBuffer::Get not found");
}


Status WriteBuffer::Delete(PutOption &, std::string &key){

    // 首先和Get一样，先income，再flush，遍历两个buffer。找到的话，修改_deleted参数
    // step1:先在flush  buffer中查找
    _lock_get_level1.lock();
    auto buffer = _buffers[_income_index];
    int buffer_len = buffer.size();
    _lock_get_level1.unlock();
    for(int i = 0; i < buffer_len; i++){
        if(strncmp(buffer[i]._key, key.c_str(), strlen(buffer[i]._key))){
            _lock_get_level1.lock();
            _lock_put_level2.lock();
            buffer[i]._is_deleted = true;
            _lock_put_level2.unlock();
            _lock_get_level1.unlock();
            return Status(STATUS_OKAY, "WriteBuffer::Delete::income OK.");
        }
    }
    // step2: 在income buffer侦破哪个查找
    _lock_get_level1.lock();
    buffer = _buffers[_flush_index];
    buffer_len = buffer.size();
    _lock_get_level1.unlock();
    for(int i = 0; i < buffer_len; i++){
        if(strncmp(buffer[i]._key, key.c_str(), strlen(buffer[i]._key))){
            _lock_get_level1.lock();
            _lock_put_level2.lock();
            buffer[i]._is_deleted = true;
            _lock_put_level2.unlock();
            _lock_get_level1.unlock();
            return Status(STATUS_OKAY, "WriteBuffer::Delete::income OK.");
        }
    }

    // TODO-04: 在storageEngine中删除一个记录


    return Status(STATUS_OKAY, "WriteBuffer::Delete OK.");
}




}