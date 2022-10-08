#include<iostream>
#include<string>
#include <sys/syslog.h>

// #include "../include/logger.h"
// #include "../include/database.h"


#include<city.h>



auto main() -> int {

    std::cout  << "====[hello minikv]====" << std::endl;

    
    std::string str = "shang";

    uint128 value = CityHash128(str.c_str(), str.size());
    std::cout << value.first << " " << value.second << std::endl;
   
    
    return 0;
}