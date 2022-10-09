#include<iostream>
#include<string>
#include <sys/syslog.h>

#include "../include/logger.h"
#include "../include/database.h"
#include "../include/hash.h"


auto main() -> int {

    std::cout  << "====[hello minikv]====" << std::endl;

    minikv::Logger::DEBUG("sdc");


    minikv::Hash *cityHash = new minikv::CityHash();
    std::string hello = "shangchao";

    std::cout << cityHash->Hash32(hello) << std::endl;

    
    return 0;
}