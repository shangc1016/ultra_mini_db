#include "../include/writeBuffer.h"
#include "../include/log.h"

#include <chrono>
#include <system_error>
#include <thread>
#include <utility>


namespace minikv {

WriteBuffer::WriteBuffer(DatabaseOptions db_options){

    _income_index = 0;
    _flush_index  = 1;

    _buffer_size[_income_index] = 0;
    _buffer_size[_flush_index ] = 0;
    
    _buffer_flush_handler = std::thread(&WriteBuffer::buffer_flush_loop, this);

    _db_options = db_options;

}


void WriteBuffer::Close(){
    _buffer_flush_handler.join();
}


// 线程运行的函数
void WriteBuffer::buffer_flush_loop(){
    for(;;){
        
        std::cout << "[thread buffer_flush] spining..." << std::endl;
        if(_buffers[_flush_index].size() >= _db_options._max_wb_buffer_size){
            // TODO: 这儿说明flush_buffer已经满了，需要交给storageEnginea来写到磁盘上了;
            std::cout << "[flushed]" << _flush_index << ":" << _buffers[_flush_index].size() << std::endl;
            _buffers[_flush_index].clear();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}


Status WriteBuffer::Get(GetOption &option, std::string &key, std::string *value){
    // 先从income这个buffer查找，
    auto buffer = _buffers[_income_index];
    for(auto it = buffer.begin(); it != buffer.end(); --it){
        // 逐个遍历，查找
        if(it->_key.compare(key) == 0){
            *value = it->_value;
            return Status(STATUS_OKAY, "WriteBuffer::Get::_imcome_index OK.");
        }
    }
    
    buffer = _buffers[_flush_index];
    for(auto it = buffer.begin(); it != buffer.end(); --it){
        if(it->_key.compare(key) == 0){
            *value = it->_value;
            return Status(STATUS_OKAY, "WriteBuffer::Get::_flush_index OK.");
        }
    }
    
    // TODO-01: 需要实现去storageEngine中查找.

    // delete after
    std::cout << option.get << std::endl;

    return Status(STATUS_NOT_FOUND, "writeBuffer::Get not found");
}


Status WriteBuffer::Put(PutOption &option, std::string &key, std::string &value){

    minikv::Logger::Trace("WriteBuffer::Put", "");

    if(_buffer_size[_income_index] >= _db_options._max_wb_buffer_size) {
        // TODO-02: 
        return Status(STATUS_BUFFER_FULL, "WriteBuffer::Put full.");
    }


    Record record;
    record._key = key;
    record._value = value;
    record._is_deleted = false;
    record._key_size = key.size();
    record._value_size = value.size();

    _write_lock.lock();
    
    _buffers[_income_index].push_back(record);
    _buffer_size[_income_index]++;

    _write_lock.unlock();

    // TODO-03: 如果buffer满了，通知thread处理

    // delete after
    std::cout << option.put << std::endl;

    return Status(STATUS_OKAY, "WriteBuffer::Put OK.");
}

Status WriteBuffer::Delete(PutOption &option, std::string &key){

    // 首先和Get一样，先income，再flush，遍历两个buffer。找到的话，修改_deleted参数
    // step1:先在flush  buffer中查找
    auto buffer = _buffers[_flush_index];
    for(auto it = buffer.end(); it != buffer.begin(); --it){
        if(it->_key.compare(key) == 0){
            _write_lock.lock();
            it->_is_deleted = true;
            _write_lock.unlock();
            return Status(STATUS_OKAY, "WriteBuffer::Delete::flush OK.");
        }
    }
    // step2: 在income buffer侦破哪个查找
    buffer = _buffers[_income_index];
    for(auto it = buffer.end(); it != buffer.begin(); --it){
        if(it->_key.compare(key) == 0){
            _write_lock.lock();
            it->_is_deleted = true;
            _write_lock.unlock();
            return Status(STATUS_OKAY, "WriteBuffer::Delete::income OK.");
        }
    }

    // TODO-04: 在storageEngine中删除一个记录

    // delete after
    std::cout << option.put << std::endl;

    return Status(STATUS_OKAY, "WriteBuffer::Delete OK.");
}




}