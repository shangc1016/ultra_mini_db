#include <city.h>
#include <cstdint>
#include <iostream>
#include <thread>

#include <gtest/gtest.h>

#include "../include/storageengine.h"
#include "../include/utils.h"
#include "../include/status.h"





TEST(engine, PutGet){

    minikv::StorageEngine engine;

    uint32_t loops = 0x100;

    std::string key;
    std::string value;
    std::string valuePlaceHolder;

    minikv::Status *status;

    for(uint32_t i =0; i < loops; i++){
        key = minikv::Utils::GenRandString(10);
        value = minikv::Utils::GenRandString(10);
        // PUT
        status = engine.Put(key, value);
        if(!status->IsOK()) {
            std::cout << status->ToString() << std::endl;
        }
        delete status;
        // GET
        status = engine.Get(key, &valuePlaceHolder);
        if(!status->IsOK()) {
            std::cout << status->ToString() << std::endl;
        }
        delete status;

        // EQ test
        EXPECT_EQ(value, valuePlaceHolder);
        value.clear();
        valuePlaceHolder.clear();
    } 
}




void threadFunc(minikv::StorageEngine *engine){

    int loops = 1000000;

    std::string key;
    std::string value;
    std::string valuePlaceHolder;

    minikv::Status *status;

    for(auto i =0; i < loops; i ++){
        key = minikv::Utils::GenRandString(10);
        value = minikv::Utils::GenRandString(10);
        // PUT
        status = engine->Put(key, value);
        
        delete status;

        // GET
        status = engine->Get(key,  &valuePlaceHolder);
        if(!status->IsOK()) {
            std::cout << status->ToString() << std::endl;
        }
        delete status;
        EXPECT_EQ(value, valuePlaceHolder);

        value.clear();
        valuePlaceHolder.clear();
    }
}


TEST(engine, multiThread){


    minikv::StorageEngine engine;

    int threadNum = 100;

    std::thread threads[threadNum];

    for(auto i = 0; i < threadNum; ++i){
        threads[i] = std::thread(threadFunc, &engine);
    }

    for(auto i = 0; i < threadNum; ++i){
        threads[i].join();
    }

}


