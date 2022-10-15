#ifndef __DATABASE_H_
#define __DATABASE_H_

#include "options.h"
#include "status.h"
#include <string>
namespace minikv {

class Database {  

public:

    Database(const DatabaseOptions&, std::string&);
    ~Database() {
        Close();
    }

    Status Open();
    void Get();
    void Put();
    void Close();
    void NewIterator();
    void Compact();   // 压缩db空间
    void Flush();     // Flush数据库的数据到磁盘





private:

    std::string _dbName;
    int         _fd;
    bool        _is_open;
    
    DatabaseOptions _dbOptions;
    std::string *_storageEngine;
    std::string *_writeBuffer;
    std::string *_eventManager;

    // 压缩
    // 校验
    // 线程

};



}

#endif