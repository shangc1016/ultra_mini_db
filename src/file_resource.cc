#include "../include/file_resource.h"

#include <city.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <experimental/filesystem>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>

#include "../include/utils.h"

namespace minikv {

// get all file's name in db_dir_path
FileResource::FileResource(DatabaseOptions db_options) {
  _db_options = db_options;

  // step1: judge whether db_dir exist.
  struct stat dirinfo;
  bool db_dir_exist = (stat(_db_options._db_path.c_str(), &dirinfo) == 0);
  if (!db_dir_exist) {
    // step1.1: do not enable create if dir not exist.
    if (!db_options._create_db_dir_if_not_exist) {
      fprintf(stderr, "[FR:FR]: db_dir not exist:%s.\n", strerror(errno));
      exit(EXIT_FAILURE);
    }

    // step1.2: create db_dir if enable.
    mode_t mkdir_mode =
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    if (mkdir(_db_options._db_path.c_str(), mkdir_mode) != 0) {
      fprintf(stderr, "[FR:FR]: mkdir error:%s.\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
  }

  // step2: check samename other file.
  if (!(dirinfo.st_mode & S_IFDIR)) {
    fprintf(stderr, "[FR:FR]: exist samename other file with `db_dir_path`.\n");
    exit(EXIT_FAILURE);
  }

  // here, db_path is legal,
  // step3: read all files name to uint8_t, and sort to _file_vec.
  RetrievalFiles();

  // step4: check whether increase proper.
  int start = 0;
  for (uint8_t val : _files_vec) {
    if (val - start != 1) {
      fprintf(stdout, "[FR:FR]:ERR: filename not strictly increase by 1.\n");
      exit(EXIT_FAILURE);
    }
    start = val;
  }

  // now, we believe that front n-1 file have no extra space, but we dont
  // know whether last file is full.
  // FIXME: what if we use tweo different size db_file_sz to open db?
  // now, simply ignore it.

  // step5: read all file's header first, and get it's _index position.

  _current_file_number = 1;
  auto filepath = _db_options._db_path + "/" +
                  Utils::MakeFixedLength(_current_file_number, 3);
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
  _mmap_record_start_pos =
      (uint64_t)ptr + _db_options._internal_hstable_header_sz;
  // 记录的当前位置，
  _mmap_record_current_pos = _mmap_record_start_pos;
}

FileResource::~FileResource() {
  auto err = munmap((void *)_mmap_start_pos, _db_options._max_single_file_size);
  if (err < 0) {
    fprintf(stderr, "FileResource:munmap error: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
}

Status FileResource::ChangeToNewFile() {
  fprintf(stdout, "FileResource::ChangeToNewFile\n");

  {
    std::lock_guard<std::mutex> lock(_change_file_lock);
    // step1: munmap old file.
    auto err =
        munmap((void *)_mmap_start_pos, _db_options._max_single_file_size);
    if (err < 0) {
      fprintf(stderr, "FileResource:munmap error: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
    // step2: increase file number.
    _current_file_number++;
    auto filepath = _db_options._db_path + "/" +
                    Utils::MakeFixedLength(_current_file_number, 3);

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
    // step3: mmap new file.
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
    _mmap_record_start_pos =
        (uint64_t)ptr + _db_options._internal_hstable_header_sz;
    // 记录的当前位置，
    _mmap_record_current_pos = _mmap_record_start_pos;
  }

  return Status(STATUS_OKAY, "");
}

void FileResource::RetrievalFiles() {
  _current_file_number = 0;

  // now db_path legal.
  for (const auto &entry : std::experimental::filesystem::directory_iterator(
           _db_options._db_path)) {
    // step1: get it's filename.
    auto filename = entry.path().filename();
    // step2: convert it to uint8_t.
    try {
      uint8_t number = std::stoi(filename);
      // _current_file_number 是最大的文件名
      if (number > _current_file_number) _current_file_number = number;
    } catch (std::invalid_argument const &ex) {
      if (filename.compare("_index") != 0) {
        exit(EXIT_FAILURE);
      }
    }
  }
}

uint64_t FileResource::GetCurrentRecordPtr() {
  return _mmap_record_current_pos;
}

uint64_t FileResource::GetMmapStartPtr() { return _mmap_start_pos; }

uint64_t FileResource::GetRecordStartPtr() { return _mmap_record_start_pos; }

void FileResource::IncreaseRecordPtr(uint64_t offset) {
  _mmap_record_current_pos += offset;
}

uint8_t FileResource::GetCurrentFileNumber() { return _current_file_number; }

bool FileResource::IsFileFull(uint64_t offset) {
  return offset >= (_mmap_start_pos + _mmap_max_offset);
}

void FileResource::IncreaseFilePos(uint64_t increment) {
  _mmap_record_current_pos += increment;
}

uint64_t FileResource::GetMmapSize() {
  return _db_options._max_single_file_size;
}

std::string FileResource::GetCurrentFilePath() {
  auto path = _db_options._db_path + "/001";
  return path;
}

}  // namespace minikv