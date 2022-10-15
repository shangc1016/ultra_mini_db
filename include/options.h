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

    // 哈希函数的选择
    HashType hash;


    // 在写入数据的时候需不需要压缩
    bool need_compress;

    // 创建数据库的时候，不需要数据库目录提前准备好
    int error_if_exist;
    int create_if_missing;

    


};


// PUT的参数,可以选择同步或者异步
struct GetOption{
    bool get;
};



// GET的参数:可以选择检查校验和
struct PutOption{
    bool put;
};






}


#endif