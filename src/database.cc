#include "../include/database.h"
#include "../include/writeBuffer.h"
#include "../include/eventmanager.h"

#include <memory>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


namespace minikv {


// 设置相关参数
Database::Database(const DatabaseOptions& dbOption, std::string dbPath){
    
    // 
    _db_options = dbOption;
    _db_name    = dbPath;
    
    _db_options._db_path = dbPath;


    Event<std::vector<Record>> _flush_event;

    // wb
    _write_buffer = new WriteBuffer(_db_options, &_flush_event);

    // se
    _storage_engine = new StorageEngine(_db_options, &_flush_event);

    _event_manager = nullptr;
}


Database::~Database() {

    delete _write_buffer;
    delete _storage_engine;

}

Status Database::Close(){

    // 关闭打开文件等，，，
    return Status(STATUS_OKAY, "Database::Close");
}


Status Database::Open(){
    // _dbName是一个目录，需要在这个目录下面创建多个存文件;
    // 检查路径是否存在，检查路径是否是个文件

    struct stat info;
    bool db_exists = (stat(_db_name.c_str(), &info) == 0);
    // 创建database目录
    if(db_exists && !(info.st_mode &  S_IFDIR)) {
        return Status(STATUS_IO_ERROR, "A file with same name of database already exists.");
    }

    if(db_exists && _db_options._error_if_exist) {
        return Status(STATUS_IO_ERROR, "The database directory already exist.");
    }
    if(!db_exists && _db_options._create_if_missing && (mkdir(_db_name.c_str(), 0755) < 0)) {
        return Status(STATUS_IO_ERROR, "Creating database directory error.");
    }
    // 创建database/dbOption文件
    // 如果dbOption文件存在，从其中读取配置选项,,,使用mmap的方式，把dbOption文件映射到内存
    // 否则从HATable中得到dbOption，
    // 如果啥都没有，就把初始化用到的dbOption写到文件中

    // 初始化哈希函数，

    // 实例化StorageEngine
    // 实例化WriteBuffer
    // 需要把线程同步的一些参数传递给两个对象

    return Status(STATUS_OKAY, "Open Database successed.");

}


Status Database::Get(std::string &key, std::string *value){
    GetOption get_option;
    // FIXME-01
    return Get(get_option, key, value);
}


Status Database::Get(GetOption &get_option, std::string &key, std::string *value){
    return _write_buffer->Get(get_option, key, value);
}


Status Database::Put(std::string &key, std::string &value){
    PutOption put_option;
    return Put(put_option, key, value);
}


Status Database::Put(PutOption &put_option, std::string &key, std::string &value){
    return _write_buffer->Put(put_option, key, value);
}


Status Database::Delete(std::string &key){
    PutOption put_option;
    return _write_buffer->Delete(put_option, key);
}



}