#ifndef INCLUDE_RECORD_H_
#define INCLUDE_RECORD_H_

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>

#include "status.h"

namespace minikv {

class Record {
 public:
  bool _is_deleted;

  const char *_key;
  const char *_val;

  uint64_t _key_size;
  uint64_t _val_size;

  uint64_t _key_margin;
  uint64_t _val_margin;

  Record() {
    _key = nullptr;
    _val = nullptr;
  }
};

// TODO(shang): 序列化的函数，反序列化的函数

}  // namespace minikv

#endif  // INCLUDE_RECORD_H_
