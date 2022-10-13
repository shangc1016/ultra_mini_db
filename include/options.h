#ifndef __OPTIONS_H_
#define __OPTIONS_H_

#include <cstdint>
namespace minikv {


// 使用的哈希算法
enum HashType{

    CityHash64,
    MurmurHash128,
};






// 包括了文件类型
struct DatabaseOptions{


    // 在持久化的时候，hstable的大小
    uint64_t internal_hstable_header_sz;
    


};


// PUT的参数


// GET的参数







}


#endif