#include "../../include/storageengine.h"
#include "../../include/utils.h"
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>





int main(){
    std::cout << "kv-client" << std::endl;
    // socket
    int client = socket(AF_INET, SOCK_STREAM, 0);
    if(client == -1){
        std::cout << "socket error" << std::endl;
        exit(EXIT_FAILURE);
    }

    // connect
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if(connect(client, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        std::cout << "connect error" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout <<"connect" << std::endl;

    char data[255];
    char buf[255];

    while(true){
        std::cin >> data;
        send(client, data, strlen(data), 0);
        if(strcmp(data, "exit") == 0){
            std::cout << "disconnect" << std::endl;
            break;
        }
        memset(buf, 0, sizeof(buf));
        int len = recv(client, buf, sizeof(buf), 0);
        buf[len] = '\0';
        std::cout << "received data: " << buf <<std::endl;
    }

    close(client);
}