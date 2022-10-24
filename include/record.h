#ifndef INCLUDE_RECORD_H_
#define INCLUDE_RECORD_H_

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#include "status.h"

namespace minikv {

class Record {
 public:
  // static function
  static Status DecodeRecord(uint64_t, Record &);
  static Status EncodeRecord(uint64_t, const Record &);
  static void Print(const Record &, bool);

  // none static function
  uint32_t GetRecordSize();

  // TODO(shang): add record validication method.

  // variable
  bool _is_deleted;

  std::vector<char> _key;
  std::vector<char> _val;

  uint64_t _key_size;
  uint64_t _val_size;

  uint64_t _key_margin;
  uint64_t _val_margin;
};

// TODO(shang): 序列化的函数，反序列化的函数

}  // namespace minikv

#endif  // INCLUDE_RECORD_H_
