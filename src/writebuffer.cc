#include "../include/writeBuffer.h"
#include "../include/log.h"

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <memory>
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

    _buffer_size[_income_index] = 0;
    _buffer_size[_flush_index ] = 0;

    _thread_running = true;

    _buffer_flush_handler = std::thread(&WriteBuffer::buffer_flush_loop, this);

    _db_options = db_options;

    _sync_event = event;

}


void WriteBuffer::Close(){

    // 退出线程
    _thread_running = false;
    // 等待线程返回
    _buffer_flush_handler.join();

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

// 线程运行的函数
void* WriteBuffer::buffer_flush_loop(){

    while(_thread_running){
        // set a threshold, if flush_buffer oversize it, then transit it to `se`.
        // TODO - when to swap `income_index` and `flush_index`
        if(_buffers[_flush_index].size() == 0 && _buffers[_income_index].size() != 0){
            // FIXME - :locking?
            std::swap(_flush_index, _income_index);
        }
        // if flush buffer oversize half of predefined max_buffer_size, then hand to se.
        if(_buffers[_flush_index].size() > _db_options._max_wb_buffer_size / 2) {
            _sync_event->SendAndWaitForDone(_buffers[_flush_index]);
            std::cout << "[WB]: finish sync." << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));    
    }
    // TODO - is thread exit successfully?
    // TODO - handle signal
    return 0;
}


Status WriteBuffer::Put(PutOption &option, std::string &key, std::string &value){

    minikv::Logger::Trace("WriteBuffer::Put", "");

    if(_buffer_size[_income_index] >= _db_options._max_wb_buffer_size) {
        // TODO-02:  need swap income_buffer and flush_buffer.
        Logger::Trace("WriteBuffer::Put", "swap income_index and flush_index");
        _swap_index_lock.lock();
        std::swap(_income_index, _flush_index);
        _swap_index_lock.unlock();
    }

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

    _write_income_lock.lock();
    
    _buffers[_income_index].push_back(record);
    _buffer_size[_income_index]++;

    _write_income_lock.unlock();


    // TODO-03: 如果buffer满了，通知thread处理

    // delete after
    std::cerr << "WriteBuffer::Put:option" << option.put << std::endl;

    return Status(STATUS_OKAY, "WriteBuffer::Put OK.");
}


Status WriteBuffer::Get(GetOption &option, std::string &key, std::string *value){
    // 先从income这个buffer查找，
    auto buffer = _buffers[_income_index];
    for(auto it : buffer){
        // 逐个遍历，查找
        if(strncmp(it._key, key.c_str(), strlen(it._key)) == 0){
            *value = std::string(it._value);
            return Status(STATUS_OKAY, "WriteBuffer::Get::income");
        }
    }
    
    buffer = _buffers[_flush_index];
    for(auto it : buffer){
        if(strncmp(it._key, key.c_str(), strlen(it._key)) == 0){
            *value = std::string(it._value);
            return Status(STATUS_OKAY, "WriteBuffer::Get::flush");
        }
    }
    
    // TODO-01: 需要实现去storageEngine中查找.

    // delete after
    std::cout << "WriteBuffer::Get::option" << option.get << std::endl;

    return Status(STATUS_NOT_FOUND, "writeBuffer::Get not found");
}


Status WriteBuffer::Delete(PutOption &option, std::string &key){

    // 首先和Get一样，先income，再flush，遍历两个buffer。找到的话，修改_deleted参数
    // step1:先在flush  buffer中查找
    auto buffer = _buffers[_flush_index];
    for(auto it = buffer.end(); it != buffer.begin(); --it){
        if(strncmp(it->_key, key.c_str(), strlen(it->_key))){
            _write_income_lock.lock();
            it->_is_deleted = true;
            _write_income_lock.unlock();
            return Status(STATUS_OKAY, "WriteBuffer::Delete::flush OK.");
        }
    }
    // step2: 在income buffer侦破哪个查找
    buffer = _buffers[_income_index];
    for(auto it = buffer.end(); it != buffer.begin(); --it){
        if(strncmp(it->_key, key.c_str(), strlen(it->_key))){
            _write_income_lock.lock();
            it->_is_deleted = true;
            _write_income_lock.unlock();
            return Status(STATUS_OKAY, "WriteBuffer::Delete::income OK.");
        }
    }

    // TODO-04: 在storageEngine中删除一个记录

    // delete after
    std::cout << "WriteBuffer::Delete::option" << option.put << std::endl;

    return Status(STATUS_OKAY, "WriteBuffer::Delete OK.");
}




}