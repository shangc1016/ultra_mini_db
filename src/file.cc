#include "../include/file.h"

#include <city.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

namespace minikv {

// get all file's name in db_dir_path
FileResource::FileResource(DatabaseOptions db_options) {
  _db_options = db_options;

  // step1: whether dir exist.
  struct stat dirinfo;

  bool db_dir_exist = (stat(_db_options._db_path.c_str(), &dirinfo) == 0);

  if (!db_dir_exist) {
    // whether enable create dir.
    if (!db_options._create_db_dir_if_not_exist) {
      fprintf(stderr, "db_dir_path not exist:%s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }

    // create db_dir.
    mode_t mkdir_mode =
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    if (mkdir(_db_options._db_path.c_str(), mkdir_mode) != 0) {
      fprintf(stderr, "mkdir error:%s.\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
  }

  // exist, but not dir
  if (!(dirinfo.st_mode & S_IFDIR)) {
    fprintf(stderr, "exist samename other file with `db_dir_path`\n");
    exit(EXIT_FAILURE);
  }

  // step2: db dir exists,
  // FIXME:
  auto filepath = _db_options._db_path + "/001";
  // std::cout << "filepath:" << filepath << std::endl;
  auto fd = open(filepath.c_str(), O_CREAT | O_RDWR, 0644);
  if (fd < 0) {
    fprintf(stderr, "FileResource::FileResource:open error:%s.\n",
            strerror(errno));
    exit(EXIT_FAILURE);
  }

  struct stat info;
  stat(filepath.c_str(), &info);
  if (info.st_size != _db_options._max_single_file_size) {
    truncate(filepath.c_str(), _db_options._max_single_file_size);
  }

  int prot = PROT_READ | PROT_WRITE;
  // printf("mmap64: size = %ld\n", _db_options._max_single_file_size);
  auto ptr = mmap64(nullptr, _db_options._max_single_file_size, prot,
                    MAP_SHARED, fd, 0);
  close(fd);
  if (ptr == nullptr) {
    fprintf(stderr, "FileResource::FileResource:mmap error:%s.\n",
            strerror(errno));
    exit(EXIT_FAILURE);
  }

  // mmap的起始地址
  _mmap_start_pos = (uint64_t)ptr;
  // mmap的size
  _mmap_max_offset = _db_options._max_single_file_size;
  // 文件layoput，header起始位置
  _mmap_header_start_pos = (uint64_t)ptr;
  // 文件layout，header的offset；
  _mmap_header_offset = _db_options._internal_hstable_header_sz;
  // 记录的起始地址，
  _mmap_record_start_pos = (uint64_t)ptr;
  // 记录的当前位置，
  _mmap_record_current_pos = (uint64_t)ptr;
}

FileResource::~FileResource() {
  auto err = munmap((void *)_mmap_start_pos, _db_options._max_single_file_size);
  if (err < 0) {
    fprintf(stderr, "FileResource:munmap error: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
}

u_int64_t FileResource::GetFilePtr() { return _mmap_record_current_pos; }

bool FileResource::IsFileFull(uint64_t offset) {
  return offset >= (_mmap_start_pos + _db_options._max_single_file_size);
}

void FileResource::IncreaseFilePos(uint64_t increment) {
  _mmap_record_current_pos += increment;
}

std::string FileResource::GetCurrentFilePath() {
  auto path = _db_options._db_path + "/001";
  return path;
}

Status FileResource::SafeMemcpy(const void *buf, size_t len) {
  if (IsFileFull(len))
    return Status(STATUS_MMAP_FULL, "FileResource::SafeMemcpy full.");
  std::memcpy((void *)GetFilePtr(), buf, len);
  IncreaseFilePos(len);

  return Status(STATUS_OKAY, "FileResource::SafeMemcpy OK.");
}

}  // namespace minikv