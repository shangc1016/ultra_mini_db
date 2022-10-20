#include "../include/hash.h"
#include <city.h>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>


#include "../3rdparty/murmurhash3.h"
#include "../include/hash.h"


namespace minikv {


uint64_t CityHash::HashFunction(const char *data, uint32_t len){
    // seed默认是0
    return CityHash64WithSeed(data, len, 0);
}

uint64_t CityHash::MaxInputSize() {
        return std::numeric_limits<int32_t>::max();
}

uint64_t MurmurHash::HashFunction(const char *data, uint32_t len){

    static char hash[16];
    static uint64_t output;
    // seed默认是0
    MurmurHash3_x64_128(data, len, 0, hash);
    memcpy(&output, hash, 8);
    return output;
}

uint64_t MurmurHash::MaxInputSize() {
    return  std::numeric_limits<int32_t>::max();
}


Hash* MakeHash(HashType hashType){

    switch (hashType){
        case MurmurHash128:
            return new MurmurHash();
        case CityHash64:
        default:
            return new CityHash();
    }
    return nullptr;
}

}