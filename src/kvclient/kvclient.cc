#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>

#include "../../include/eventmanager.h"
#include "../../include/file.h"
#include "../../include/storageengine.h"
#include "../../include/utils.h"

minikv::Event<int> event;

bool run = true;

void* loop() {
  for (;;) {
    if (run) printf("value =.\n");
  }
}

int main() {
  std::thread thread(loop);

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  run = false;

  thread.join();

  // minikv::Record record;

  // int fd = open("/tmp/minikv/file-01", O_RDONLY, 0644);
  // if(fd < 0){
  //     fprintf(stderr, "open error: %s\n", strerror(fd));
  //     exit(EXIT_FAILURE);
  // }
  // int len;
  // while(true) {
  //     len = read(fd, &record, sizeof(record));
  //     if(len == 0) break;
  //     if(len != sizeof(record)) break;
  //     std::cout << record._key << "; " << record._value << "; " <<
  //     record._is_deleted  << std::endl;
  // }

  // // socket
  // int client = socket(AF_INET, SOCK_STREAM, 0);
  // if(client == -1){
  //     std::cout << "socket error" << std::endl;
  //     exit(EXIT_FAILURE);
  // }

  // // connect
  // struct sockaddr_in server_addr;
  // server_addr.sin_family = AF_INET;
  // server_addr.sin_port = htons(8000);
  // server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  // if(connect(client, (struct sockaddr*)&server_addr, sizeof(server_addr)) <
  // 0){
  //     std::cout << "connect error" << std::endl;
  //     exit(EXIT_FAILURE);
  // }

  // std::cout <<"connect" << std::endl;

  // char data[255];
  // char buf[255];

  // while(true){
  //     std::cin >> data;
  //     send(client, data, strlen(data), 0);
  //     if(strcmp(data, "exit") == 0){
  //         std::cout << "disconnect" << std::endl;
  //         break;
  //     }
  //     memset(buf, 0, sizeof(buf));
  //     int len = recv(client, buf, sizeof(buf), 0);
  //     buf[len] = '\0';
  //     std::cout << "received data: " << buf <<std::endl;
  // }

  // close(client);
}
