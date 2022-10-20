#include "../../include/database.h"
#include "../../include/log.h"
#include "../../include/utils.h"


#include <chrono>
#include <cstdlib>
#include <cstring>
#include <gtest/gtest.h>
#include <netinet/in.h>
#include <ratio>
#include <sys/types.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include "../../include/eventmanager.h"
// #include "../../include/file.h"


using namespace minikv;


int main(){

    // _max_wb_buffer_size default set to 100;
    DatabaseOptions db_options;
    
    Database db(db_options, "/tmp/minikv");

    std::string key;
    std::string value;
    std::string value_place_holder;

    minikv::Logger::SetLogLevel(minikv::LogTrace);

    int loops = 1000;

    for(auto i = 0; i < loops; i++){

        key   = Utils::GenRandString(9);
        value = Utils::GenRandString(9);

        auto status = db.Put(key, value);
        if(!status.IsOK()) std::cout << status.ToString() << std::endl;

        status = db.Get(key, &value_place_holder);
        if(!status.IsOK()) std::cout << status.ToString() << std::endl;

        if(value.compare(value_place_holder) != 0){
            std::cout << "[not equal]" << std::endl;
        } 
    }


    







}



