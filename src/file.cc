#include "../include/file.h"


#include <city.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <dirent.h>



namespace minikv {


// get all file's name in db_dir_path
FileResource::FileResource(DatabaseOptions db_options){


    _db_options  = db_options;

    // step1: whether dir exist.
    struct stat dirinfo;

    bool db_dir_exist = (stat(_db_options._db_path.c_str(), &dirinfo) == 0);

    if(!db_dir_exist) {
        // whether enable create dir.
        if(!db_options._create_db_dir_if_not_exist) {
            fprintf(stderr, "db_dir_path not exist:%s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        // create db_dir.
        mode_t mkdir_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
        if(mkdir(_db_options._db_path.c_str(), mkdir_mode) != 0){
            fprintf(stderr, "mkdir error:%s.\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    // exist, but not dir
    if(!(dirinfo.st_mode & S_IFDIR)) {
        fprintf(stderr, "exist samename other file with `db_dir_path`\n");
        exit(EXIT_FAILURE);
    }

    // step2: db dir exists, 
    auto filepath = _db_options._db_path.append("/").append("001");
    // std::cout << "filepath:" << filepath << std::endl;
    auto fd = open(filepath.c_str(), O_CREAT | O_RDWR , 0644);
    if(fd < 0){
        fprintf(stderr, "FileResource::FileResource:open error:%s.\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct stat info;
    stat(filepath.c_str(), &info);
    if(info.st_size != _db_options._max_single_file_size){
        truncate(filepath.c_str(), _db_options._max_single_file_size);
    }

    int prot = PROT_READ | PROT_WRITE;
    auto ptr = mmap64(nullptr, _db_options._max_single_file_size, prot, MAP_SHARED, fd, 0);
    close(fd);
    if(ptr == nullptr){
        fprintf(stderr, "FileResource::FileResource:mmap error:%s.\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    _mmap_start_pos = (uint64_t)ptr;
    _mmap_current_pos = (uint64_t)ptr;

}



FileResource::~FileResource(){
    munmap((void*)_mmap_start_pos, _db_options._max_single_file_size);
}


u_int64_t FileResource::GetFilePtr(){
    return _mmap_current_pos;
}

bool FileResource::IsFileFull(u_int64_t offset){
    return offset > (_mmap_start_pos + _db_options._max_single_file_size);
}

std::string FileResource::GetCurrentFilePath() {
    std::string path;
    path.append(_db_options._db_path);
    // TODO:
    // path.append(_file_num);
    
    return path;
}


}