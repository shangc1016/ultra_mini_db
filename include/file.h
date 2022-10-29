#ifndef INCLUDE_FILE_H_
#define INCLUDE_FILE_H_

#include <city.h>
#include <sys/mman.h>
#include <sys/types.h>

#include <cstddef>
#include <cstdint>
#include <map>
#include <mutex>
#include <string>
#include <vector>

#include "options.h"
#include "status.h"

namespace minikv {

// 首先分析一下，把KVs写入到一个文件中，文件如果满了，就换一个新的文件继续写入。
// 之前的文件就变成了只读文件。这样的话可行吗？
class FileResource {
 public:
  FileResource(DatabaseOptions);

  ~FileResource();

  // static mmap, munmap function.
  // static uint64_t MmapFile(const std::string);
  // static void MunmapFile();

  std::string GetCurrentFilePath();

  // get this ptr, then to decode record
  uint64_t GetCurrentRecordPtr();

  // call this after encode
  void IncreaseRecordPtr(uint64_t);

  uint8_t GetCurrentFileNumber();

  bool IsFileFull(uint64_t);

  uint64_t GetRecordStartPtr();

  uint64_t GetMmapStartPtr();

  uint64_t GetMmapSize();

  // Status ChangeToNewFile();

 private:
  void IncreaseFilePos(uint64_t);

  std::mutex _change_file_lock;

  // step1: 检索合法的db文件名
  void RetrievalFiles();

  DatabaseOptions _db_options;
  //
  uint8_t _current_file_number;

  // std::vector<uint8_t> _files_vec;

  std::map<uint8_t, void*> _files_mmap_vec;

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