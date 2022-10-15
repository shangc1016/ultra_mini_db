#include "../include/database.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


namespace minikv {


// 设置相关参数
Database::Database(const DatabaseOptions &option, std::string &dbName){
    _dbOptions = option;
    _dbName    = dbName;
    _is_open   = false;
}


Status Database::Open(){
    // _dbName是一个目录，需要在这个目录下面创建多个存文件;
    // 检查路径是否存在，检查路径是否是个文件

    struct stat info;
    bool db_exists = (stat(_dbName.c_str(), &info) == 0);
    // 创建database目录
    if(db_exists && !(info.st_mode &  S_IFDIR)) {
        return Status(STATUS_IO_ERROR, "A file with same name of database already exists.");
    }

    if(db_exists && _dbOptions.error_if_exist) {
        return Status(STATUS_IO_ERROR, "The database directory already exist.");
    }
    if(!db_exists && _dbOptions.create_if_missing && (mkdir(_dbName.c_str(), 0755) < 0)) {
        return Status(STATUS_IO_ERROR, "Creating database directory error.");
    }
    // 创建database/dbOption文件
    // 如果dbOption文件存在，从其中读取配置选项,,,使用mmap的方式，把dbOption文件映射到内存
    // 否则从HATable中得到dbOption，
    // 如果啥都没有，就把初始化用到的dbOption写到文件中

    // 初始化哈希函数，

    return Status(STATUS_OKAY, "Open Database successed.");
}







}