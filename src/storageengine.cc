#include "../include/storageengine.h"
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <sys/types.h>
#include <city.h>
#include <iostream>
#include <thread>
#include <vector>
#include <cstdio>


namespace minikv {


StorageEngine::StorageEngine(DatabaseOptions db_option, Event<std::vector<Record>>* event){
    
    _db_options = db_option;
    _sync_event = event;

    // 启动线程
    _thread_running = true; 
    _buffer_store_handler = std::thread(&StorageEngine::buffer_store_loop, this);

}

StorageEngine::~StorageEngine(){

}

Status StorageEngine::Close(){
    return Status(STATUS_OKAY, "StorageEngine::Close OK.");
}


void* StorageEngine::buffer_store_loop(){
    while(_thread_running){
        auto buffered_data = _sync_event->Wait();
        // TODO:
        // step1: hash it, 
        // step2: write it to mmaped file address space, 
        // step3: for each item in `buffered_data`, get coupled data -> <hashed-key, location>
        // where location contain info about `which file` and `what offset within the file`.
        // the list we get is called `index`
        // next step: considerate how and where to persist those `index`.
    }

    return 0;
}








}