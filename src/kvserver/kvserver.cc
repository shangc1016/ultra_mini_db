#include <chrono>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <thread>
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

#include <condition_variable>


using namespace minikv;



// 线程之间用的互斥量
std::mutex worker_mutex;
std::mutex main_mutex;
// 条件变量
std::condition_variable cv_ready;
std::condition_variable cv_done;
// 临界区数据
std::string data;
// 两个临界区变量
bool data_ready = false;
bool data_processed = false;


void ThreadWork(){

    for(;;){
        std::unique_lock<std::mutex> lock(main_mutex);
        printf("[worker thread] wait main thread's data.\n");

        cv_ready.wait(lock);

        std::ostringstream oss;
        oss << std::this_thread::get_id();
        printf("[worker thread]:%s get chance to processing data.\n", oss.str().c_str());

        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        printf("worker thread complete processing data, notify main thread.\n");
        // data_processed = true;
        cv_done.notify_one();
        lock.unlock();
        // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

}

int main(){


    std::thread worker1(ThreadWork);
    std::thread worker2(ThreadWork);

    int data = 0;

    for(;;) {

        std::unique_lock<std::mutex> lock(main_mutex);

        printf("main thread preparing data.\n");

        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        // data_ready = true;
        cv_ready.notify_one();
        
        printf("main thread notify child work to processing data.\n");

        
        printf("main thread waiting for child to complete.\n");

        cv_done.wait(lock);
         
        printf("data [%d] processed.\n", data);
        // data_ready = false;
        // data_processed = false;
        data++;
    }


    worker1.join();
    worker2.join();
    


}