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

namespace minikv {


/*
从<hashed-kay:64, location:64>中得反向计算得到key的位置

*/



class StorageEngine{

public:

    StorageEngine(DatabaseOptions);
    ~StorageEngine();

    Status Close();


    





private:


    void ThreadLoop(){
        
    }

    std::thread     _store_loop_handler;
    
    DatabaseOptions _db_options;

};





}


#endif