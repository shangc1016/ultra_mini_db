#ifndef __FILE_H_
#define __FILE_H_

#include <city.h>
#include <cstdint>
#include <string>
#include <sys/mman.h>
#include <sys/types.h>
#include <vector>

#include "options.h"

namespace minikv {


// 单个文件

class FileResource{

public:

    FileResource(DatabaseOptions);

    ~FileResource();

    u_int64_t GetFilePtr();

    bool IsFileFull(u_int64_t);

    std::string GetCurrentFilePath();

private:

    DatabaseOptions _db_options;

    unsigned long _file_num;

    // bool _is_file_open;
    // int _fd;
    
    uint64_t _mmap_start_pos;
    uint64_t _mmap_current_pos;

    // max file size = _db_options._max_single_file_size
    

};


}

#endif