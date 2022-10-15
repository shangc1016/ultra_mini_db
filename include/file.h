#ifndef __FILE_H_
#define __FILE_H_

#include <cstdint>
#include <string>
#include <sys/mman.h>
#include <vector>

namespace minikv {

class FileResource{


public:

    // 返回新的文件名
    std::string GenNewFileName();

    uint64_t Mmap(){
        
        // truncate(fd, new_file_size);
        // void *ptr = mmap(0, file_length, int prot, int flags, int fd, __off_t offset)
        // ...
        return 0;
    }
    // 


private:

    // 存储的所有文件都在这个目录下
    std::string _dbName;
    std::vector<std::string> _dbFiles;
    int _currentFd;
    int _curretnFile;
    uint64_t _file_start_offset;    // mmap返结果
    uint64_t _file_current_offset;  // 当前正在使用的mmap位置 
    uint64_t file_max_size;         // _file_current_offset - _file_start_offset > file_max_size 的话，就需要flush这个文件，并且写一个新的文件







};


}

#endif