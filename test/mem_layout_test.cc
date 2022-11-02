#include <city.h>
#include <fcntl.h>
#include <gtest/gtest.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

#include "../include/database.h"
#include "../include/status.h"
#include "../include/storageengine.h"
#include "../include/utils.h"

void* MakeMmap(std::string filepath, uint32_t filesize) {
  int fd;
  void* mmap_ptr;
  struct stat info;
  //   `open`
  if ((fd = open(filepath.c_str(), O_CREAT | O_RDWR, 0644)) < 0) {
    fprintf(stderr, "[MakeMmap]: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  //   `truncate` if necessary.
  stat(filepath.c_str(), &info);
  if (info.st_size < filesize) {
    if (truncate(filepath.c_str(), filesize) < 0) {
      fprintf(stderr, "[MakeMmap:truncate]: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
  }

  //   `mmap`
  int prot = PROT_READ | PROT_WRITE;
  if ((mmap_ptr = mmap64(0, filesize, prot, MAP_SHARED, fd, 0)) == (void*)-1) {
    fprintf(stderr, "[MakeMmap:mmap64]: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  return mmap_ptr;
}

void CleanupMmap(void* mmaptr, uint32_t filesize) {
  if (munmap(mmaptr, filesize) < 0) {
    fprintf(stderr, "[CleanupMmap]: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
}