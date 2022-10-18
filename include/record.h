#ifndef __RECORD_H_
#define __RECORD_H_

#include <cstdint>
#include <cstring>
#include <string>

namespace minikv {


struct Record{

    // 此结构体在文件中的偏移
    uint64_t    _offset;

    // key、value在文件中的偏移
    uint64_t    _key_offset;
    uint64_t    _value_offset; 

    // 暂时表示，应该以指针的形式存在
    // TODO-
    char _key[20];
    char _value[20];

    // 存的实际size
    uint64_t    _key_size;
    uint64_t    _value_size;

    bool        _is_deleted;
};



// TODO-xx: 序列化的函数，反序列化的函数






}

#endif