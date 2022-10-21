#ifndef __FILE_H_
#define __FILE_H_

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

  Status SafeMemcpy(const void*, size_t);

  // 用来读数据
  u_int64_t GetFilePtr();

 private:
  bool IsFileFull(u_int64_t);

  void IncreaseFilePos(uint64_t);

  DatabaseOptions _db_options;

  uint64_t _mmap_start_pos;
  uint64_t _mmap_max_offset;

  uint64_t _mmap_header_start_pos;
  uint64_t _mmap_header_offset;

  uint64_t _mmap_record_start_pos;
  uint64_t _mmap_record_current_pos;

  // max file size = _db_options._max_single_file_size
};

}  // namespace minikv

#endif