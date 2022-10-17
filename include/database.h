#ifndef __DATABASE_H_
#define __DATABASE_H_

#include "options.h"
#include "status.h"
#include "storageengine.h"
#include "writeBuffer.h"
#include <string>
namespace minikv {





class Database {  

public:

    Database(const DatabaseOptions&, std::string);
    ~Database();

    Status Close();
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

    std::string _db_name;
    int         _fd;
    bool        _is_open;
    
    DatabaseOptions  _db_options;
    StorageEngine   *_storage_engine;
    WriteBuffer     *_write_buffer;
    std::string     *_event_manager;

    // 压缩
    // 校验
    // 线程

};



}

#endif