#ifndef __DATABASE_H_
#define __DATABASE_H_

#include <string>
namespace minikv {

class Database{





private:

    std::string dbName;
    int fd;
    bool fileopen;
    // 使用open、mmap等函数
    std::string dbOptions;
    std::string *storageEngine;
    std::string *writeBuffer;
    std::string *eventManager;

    // 压缩
    // 校验
    // 线程

};



}

#endif