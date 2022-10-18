#ifndef __STORAGEENGINE_H_
#define __STORAGEENGINE_H_

#include "options.h"
#include "status.h"
#include <city.h>
#include <cstdint>
#include <fstream>
#include <string>
#include <mutex>
#include <vector>
#include <thread>

#include "record.h"
#include "eventmanager.h"

namespace minikv {


/*
从<hashed-kay:64, location:64>中得反向计算得到key的位置

*/



class StorageEngine{

public:

    StorageEngine(DatabaseOptions, Event<std::vector<Record>>*);
    ~StorageEngine();

    Status Close();


    





private:

    // 写数据到文件
    void* buffer_store_loop();

    std::thread     _buffer_store_handler;

    bool _thread_running;

    Event<std::vector<Record>>* _sync_event;
    
    DatabaseOptions _db_options;

};





}


#endif