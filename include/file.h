#ifndef INCLUDE_FILE_H_
#define INCLUDE_FILE_H_

#include <city.h>
#include <sys/mman.h>
#include <sys/types.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "options.h"
#include "status.h"

namespace minikv {

struct FileHeader {
  uint64_t _record_nums;
  uint64_t _chechsum;
  uint64_t _version;
  uint64_t _header_sz;
};

// 单个文件
class FileResource {
 public:
  FileResource(DatabaseOptions);

  ~FileResource();

  std::string GetCurrentFilePath();

  // 用来读数据
  uint64_t GetCurrentRecordPtr();

  void IncreaseRecordPtr(uint64_t);

  uint32_t GetCurrentFileNumber();

  bool IsFileFull(uint64_t);

 private:
  void IncreaseFilePos(uint64_t);

  DatabaseOptions _db_options;

  uint32_t _file_number;

  uint64_t _mmap_start_pos;
  uint64_t _mmap_max_offset;

  uint64_t _mmap_header_start_pos;
  uint64_t _mmap_header_offset;

  uint64_t _mmap_record_start_pos;
  uint64_t _mmap_record_current_pos;

  // max file size = _db_options._max_single_file_size
};

}  // namespace minikv

#endif  // INCLUDE_FILE_H_

/*

使用mmap的过程：
1、IsFileFull判断是否mmap满了
2、GetCurrentRecordPtr得到当前mmap的record的指针
3、memcpy或者其他的encode函数
4、IncreaseRecordPtr指针移位

*/