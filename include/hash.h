#ifndef __HASH_H_
#define __HASH_H_

#include <cstdint>
#include <iostream>
#include <city.h>

namespace minikv {

class Hash {

public:
    virtual uint64_t Hash64(std::string&) = 0;
    virtual uint32_t Hash32(std::string&) = 0;

};


class CityHash : public Hash {

public:
    uint64_t Hash64(std::string &str){
        std::cout << "cityhash64" << std::endl;
        return CityHash64(str.c_str(), str.size());
    }
    uint32_t Hash32(std::string &str){
        std::cout << "cityhash32" << std::endl;
        return CityHash32(str.c_str(), str.size());
    }
};





}


#endif