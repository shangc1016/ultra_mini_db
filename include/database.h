#ifndef __DATABASE_H_
#define __DATABASE_H_

#include "eventmanager.h"
#include "options.h"
#include "status.h"
#include "storageengine.h"
#include "writebuffer.h"

#include <string>
#include <vector>

namespace minikv {





class Database {  

public:

    Database(const DatabaseOptions&, std::string);
    ~Database();

    // Status Close();
    Status Open();
    

    Status Get(std::string&, std::string*);
    Status Get(GetOption&, std::string&, std::string*);

    Status Put(std::string&, std::string&);
    Status Put(PutOption&, std::string&, std::string&);


    Status Delete(std::string&);



    // TODO-
    Status NewIterator();
    Status Compact();   // 压缩db空间
    Status Flush();     // Flush数据库的数据到磁盘'



private:

    int         _fd;
    bool        _is_open;   // 为啥有这个参数?
    
    DatabaseOptions  _db_options;
    StorageEngine   *_storage_engine;
    WriteBuffer     *_write_buffer;
    
    Event<std::vector<Record>> *_flush_event;

    // 压缩
    // 校验
    // 线程

};



}

#endif