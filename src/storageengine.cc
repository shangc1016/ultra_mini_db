#include "../include/storageengine.h"
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <sys/types.h>
#include <city.h>
#include <iostream>
#include <vector>
#include <cstdio>


namespace minikv {


StorageEngine::StorageEngine(DatabaseOptions db_option){
    _db_options = db_option;
}

StorageEngine::~StorageEngine(){
    Close();
}

Status StorageEngine::Close(){
    return Status(STATUS_OKAY, "StorageEngine::Close OK.");
}







}