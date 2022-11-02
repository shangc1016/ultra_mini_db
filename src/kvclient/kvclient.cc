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
#include "../../include/file_resource.h"
#include "../../include/storageengine.h"
#include "../../include/utils.h"

minikv::Event<int> event;

int main() {
  std::cout << "+++++ kvclient +++++" << std::endl;
  std::cout << "+++++ kvclient +++++" << std::endl;
}
