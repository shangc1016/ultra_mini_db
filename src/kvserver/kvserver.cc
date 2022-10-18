#include "../../include/database.h"
#include "../../include/log.h"
#include "../../include/utils.h"


#include <chrono>
#include <cstdlib>
#include <cstring>
#include <gtest/gtest.h>
#include <netinet/in.h>
#include <ratio>
#include <sys/types.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include "../../include/eventmanager.h"


using namespace minikv;





Event<int> _event_manager;


void* thread_func(){
    int value = _event_manager.Wait();
    value++;
    std::cout << "in thread, value = " << value << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    _event_manager.Done();
    return 0;
}


int main(){



    // th.join();
    // socket
    // int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    // if(listenfd == -1){
    //     std::cout << "socket error." << std::endl;
    //     exit(EXIT_FAILURE);
    // }

    // // bind
    // struct sockaddr_in addr;
    // addr.sin_family = AF_INET;
    // addr.sin_port = htons(8000);
    // addr.sin_addr.s_addr = INADDR_ANY;
    // if(bind(listenfd, (struct sockaddr*)&addr, sizeof(addr)) == -1){
    //     std::cout << "bind error" << std::endl;
    //     exit(EXIT_FAILURE);
    // }
    // // listen
    // if(listen(listenfd, 10) == -1){
    //     std::cout << "listen error" << std::endl;
    //     exit(EXIT_FAILURE);
    // }
    // // accept
    // int conn;
    // char client_ip[INET_ADDRSTRLEN] = "";
    // struct sockaddr_in client_addr;
    // socklen_t client_addr_len = sizeof(client_addr);
    // while(true){
    //     std::cout << "listening" << std::endl;
    //     conn = accept(listenfd, (struct sockaddr*)&client_addr, &client_addr_len);
    //     if(conn < 0) {
    //         std::cout << "accept error" << std::endl;
    //         continue;
    //     }

    //     inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    //     std::cout << "...connect " << client_ip << ":" << ntohs(client_addr.sin_port) << std::endl;

    //     char buf[255];
    //     while(true){
    //         memset(buf, 0, sizeof(buf));
    //         int len = recv(conn, buf, sizeof(buf), 0);
    //         if(len == 0) break;
    //         buf[len] = '\0';
    //         if(strcmp(buf, "exit") == 0){
    //             std::cout << "disconnect..." << client_ip << ":" << ntohs(client_addr.sin_port) << std::endl;
    //             break;
    //         }
    //         std::cout << "buf: " << buf << std::endl;
    //         send(conn, buf, len, 0);
    //     }
    //     close(conn);
    // }

    // close(listenfd);

    // 专门的线程处理accept，然后把连接交给线程池

    
    // 默认构造函数
    // writebuffer最大100
    DatabaseOptions db_options;
    
    Database db(db_options, "/tmp/minikv");

    Status status;

    std::string key;
    std::string value;
    std::string value_place_holder;

    minikv::Logger::SetLogLevel(minikv::LogTrace);

    int loops = 0;

    for(auto i = 0; i < loops; i++){
        // std::cout << "[put-get]: " << i << std::endl;
        printf("[put-get]: %d\n", i);
        key = minikv::Utils::GenRandString(9);
        value = Utils::GenRandString(9);

        status = db.Put(key, value);
        if(!status.IsOK()) std::cout << status.ToString() << std::endl;

        status = db.Get(key, &value_place_holder);
        if(!status.IsOK()) std::cout << status.ToString() << std::endl;

        if(value.compare(value_place_holder) != 0){
            std::cout << "not equal." << std::endl;
            std::cout << "value:" << value << std::endl;
            std::cout << "value_place_holder:" << value_place_holder << std::endl;
        }
    }


    std::this_thread::sleep_for(std::chrono::milliseconds(10000));

    db.Close();

}





/*
// 一个用在发送数据阶段、一个用在接收数据阶段
// 这个是互相的；
// 一个标记发送数据
std::mutex mutex1;
// 一个标记接收数据
std::mutex mutex2;

std::condition_variable cv;

bool data_ready;
bool data_processed;

std::string data = "Original data.";


void WorkerThread(){

    std::unique_lock<std::mutex> receive_lock(mutex1);

    printf("[worker thread]: waiting data from main thread.\n");

    cv.wait(receive_lock, []{return data_ready;});

    receive_lock.unlock();


    printf("[worker thread]: get data, processing...\n");

    data += "after worker thread process.";

    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::unique_lock<std::mutex> send_lock(mutex2);

    data_processed = true;

    send_lock.unlock();

    cv.notify_one();

} 


int main(){

    std::thread worker(WorkerThread);

    printf("[  main thread]: preparing data\n");

    std::unique_lock<std::mutex> send_lock(mutex1);

    data_ready = true;
    
    send_lock.unlock();
    
    cv.notify_one();

    printf("[  main thread]: sent data\n");

    printf("[  main thread]: waiting for processed data.\n");
    
    // 当前线程先睡眠，会导致worker线程先拿到mutex2的锁，说明即使先notify，再wait也是正常执行的
    // 因为notify的话，说明"条件"满足了，另一个等待的线程不需要wait休眠等待了。
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    std::unique_lock<std::mutex> receive_lock(mutex2);

    // 这儿的wait(unique_lock, predicate)，如果predicate满足。不会wait阻塞
    cv.wait(receive_lock, []{ return data_processed;});

    receive_lock.unlock();

    printf("[  main thread]: data after process: %s.\n", data.c_str());

    worker.join();

}


*/