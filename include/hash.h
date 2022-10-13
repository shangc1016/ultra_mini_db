#ifndef __HASH_H_
#define __HASH_H_

#include <city.h>
#include <cstdint>
#include <limits>

#include "options.h"


namespace minikv {

// 哈希部分

class Hash {

public: 
    Hash() {}
    virtual ~Hash() {}
    virtual uint64_t HashFunction(const char*, uint32_t) = 0;
    virtual uint64_t MaxInputSize() = 0;

};


// CityHash
class CityHash: public Hash {

public:
    CityHash() {}
    virtual ~CityHash() {}
    virtual uint64_t HashFunction(const char*, uint32_t);
    virtual uint64_t MaxInputSize();
};

// MurmurHash
class MurmurHash :public Hash {

public:
    MurmurHash() {}
    virtual ~MurmurHash() {}
    virtual uint64_t HashFunction(const char*, uint32_t);
    virtual uint64_t MaxInputSize();
};


Hash* MakeHash(HashType hashType);











}

#endif
