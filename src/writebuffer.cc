#include "../include/writeBuffer.h"
#include <chrono>
#include <system_error>
#include <thread>
#include <utility>


namespace minikv {

WriteBuffer::WriteBuffer(){

    _income_index = 0;
    _flush_index = 1;
    _size[_income_index] = 0;
    _size[_flush_index] = 0;
    _buffer_size = 10;
    buffer_flush_handler = std::thread(&WriteBuffer::buffer_flush_loop, this);
}


void WriteBuffer::Close(){
    buffer_flush_handler.join();
}


// 线程运行的函数
void WriteBuffer::buffer_flush_loop(){
    for(;;){
        // mock
        std::cout << "buffer_flush_loop: +flush_index = " << _flush_index << std::endl;
        if(_buffers[_flush_index].size() >= _buffer_size){
            // TODO: 这儿说明flush_buffer已经满了，需要交给storageEnginea来写到磁盘上了;
            std::cout << "[flushed]" << _flush_index << ":" << _buffers[_flush_index].size() << std::endl;
            _buffers[_flush_index].clear();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}


Status WriteBuffer::Get(GetOption &option, std::string &key, std::string *value){

    // 现在income这个buffer查找，反向查找
    auto buf = _buffers[_income_index];
    for(auto it = buf.end(); it != buf.begin(); --it){
        if(it->first.compare(key) == 0) {
            *value = it->second;
            return Status(STATUS_OKAY, "");
        }
    }
    buf = _buffers[_flush_index];
    for(auto it = buf.end(); it != buf.begin(); --it){
        if(it->first.compare(key) == 0){
            *value = it->second;
            return Status(STATUS_OKAY, "");
        }
    }

    std::cout << option.get << std::endl;

    return Status(STATUS_NOT_FOUND, "writeBuffer [get] not found");
}


Status WriteBuffer::Put(PutOption &option, std::string &key, std::string &value){

    if(_buffers[_income_index].size() < _buffer_size) {
        _write_lock.lock();
        _buffers[_income_index].push_back(std::make_pair(key, value));
        std::cout << "writeBuffer[Put]:" << key << std::endl;
        _write_lock.unlock();
    }  else if(_buffers[_income_index].size() >= _buffer_size) {
        std::swap(_income_index, _flush_index);
        // std::cout << "income_index = " << _income_index << ";\tflush_index = " << _flush_index << std::endl;
        // std::cout << "--swap executed" << std::endl;
    } else {
        return Status(STATUS_NO_SPACE, "writeBuffer:[PUT], full");
    }

    option.put = false;


    // std::cout << option.put << std::endl;

    return Status(STATUS_OKAY, "");
}

Status WriteBuffer::Delete(PutOption &, std::string &){

    return Status(STATUS_OKAY, "");
}




}