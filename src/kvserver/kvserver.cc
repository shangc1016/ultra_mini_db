#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "../../include/storageengine.h"
#include "../../include/utils.h"
#include "../../include/hash.h" 
#include "../../include/log.h"
#include "../../include/writeBuffer.h"


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <map>

using namespace minikv;



void InsertWriteBuffer(WriteBuffer* _wb) {

    std::string key;
    std::string value;
    std::string valuePlaceHolder;

    PutOption putOption;
    GetOption getOption;

    for(;;) {
        key = Utils::GenRandString(10);
        value = Utils::GenRandString(10);
        auto status = _wb->Put(putOption, key, value);
        if(!status.IsOK()){
            std::cout << status.ToString() << std::endl;
        }
        status = _wb->Get(getOption, key, &valuePlaceHolder);
        if(!status.IsOK()) {
            std::cout << status.ToString() << std::endl;
        }
        if(valuePlaceHolder.compare(value) != 0){
            std::cout << "put get not equal" << std::endl;
        }
    }
}




// kvservber main
int main() {



    std::string filename = "/tmp/minikv/log";

    int fd;

    fd = open(filename.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP  |S_IROTH | S_IWOTH);
    if(fd == -1){
       printf("file open error:%s\n", strerror(errno));
    }

    struct stat fileinfo;
    stat(filename.c_str(), &fileinfo);

    printf("filesize = %ld\n", fileinfo.st_size);

    int ret = truncate(filename.c_str(), 4096 * 10);
    if(ret != 0){
        printf("truncate error, %s\n", strerror(errno));
    }

    void *ptr = mmap(0, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if(ptr == MAP_FAILED) {
        close(fd);
        printf("mmap failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    close(fd);

    std::string name = "shangchao";

    strncpy((char*)ptr, name.c_str(), name.size());

    strncpy((char*)((u_int64_t)ptr + 1024), name.c_str(), name.size());

    printf("ptr = %p\n", ptr);

    // char name[10];

    // strncpy(name, (char*)ptr, 10);
    // std::cout << name << std::endl;

    // strncpy(name, (char*)(u_int64_t)ptr + 1024, 10);
    // std::cout << name << std::endl;

    ret = munmap(ptr,  1024);
    printf("munmap ret = %d", ret);



    return 0;
}

