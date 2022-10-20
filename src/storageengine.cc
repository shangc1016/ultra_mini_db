#include "../include/storageengine.h"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <ios>
#include <ratio>
#include <city.h>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <vector>
#include <cstdio>

#include <inttypes.h>
#include <sys/types.h>


namespace minikv {


StorageEngine::StorageEngine(DatabaseOptions db_option, Event<std::vector<Record>>* event){
    
    _db_options = db_option;
    _sync_event = event;

    // flag to start thread, and stop it.
    _thread_running = true; 
    // pass a class method to std::thread, add class object this.
    _buffer_store_handler = std::thread(&StorageEngine::buffer_store_loop, this);

    if(_buffer_store_handler.joinable()){
        _buffer_store_handler.detach();
    }

    // 文件资源，用来持久化kv数据，使用mmap的方式
    _file_resource = new FileResource(db_option);

    // hash 
    switch (_db_options._hash_type) {
        case MurmurHash128:
            _hash = new MurmurHash();
            break;
        case CityHash64:
        default:
            _hash = new CityHash();
    }

}


void StorageEngine::Stop(){
    _thread_running = false;
}


// TODO:
// step1: hash it, 
// step2: write it to mmaped file address space, and get the location where it exist.
// step3: for each item in `buffered_data`, get coupled data -> <hashed-key, location>
// where location contain info about `which file` and `what offset within the file`.
// the list we get is called `index`
// next step: considerate how and where to persist those `index`.
void* StorageEngine::buffer_store_loop(){
    
    auto fd = open("001", O_CREAT | O_WRONLY | O_APPEND, 0644);
    if(fd < 0) {
        fprintf(stderr, "open error:%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }


    while(true){
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        if(_thread_running == false) break;
        
        // printf("se thread loop.\n");
        auto buffer = _sync_event->Wait();
        // printf("se have lock\n");

        for(auto item : buffer){
            // 计算hash-key，底层
            fprintf(fdopen(fd, "w"), "%" PRIu64 "\n", _hash->HashFunction(item._key, strlen(item._key)));
        }
        fflush(fdopen(fd, "w"));
        _sync_event->Done();
        // printf("se have notuify wb\n");


    }

    close(fd);

    return 0;
}








}