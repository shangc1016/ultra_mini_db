#ifndef INCLUDE_HASH_H_
#define INCLUDE_HASH_H_

#include <city.h>

#include <cstdint>
#include <limits>

#include "options.h"

namespace minikv {

// 哈希部分

class Hash {
 public:
  Hash() {}
  virtual ~Hash() {}
  virtual uint64_t HashFunction(const char*, uint32_t) = 0;
  virtual uint64_t MaxInputSize() = 0;
};

// CityHash
class CityHash : public Hash {
 public:
  CityHash() {}
  ~CityHash() override {}
  // override用在派生类中，说明这个方法是重构的
  uint64_t HashFunction(const char*, uint32_t) override;
  uint64_t MaxInputSize() override;
};

// MurmurHash
class MurmurHash : public Hash {
 public:
  MurmurHash() {}
  virtual ~MurmurHash() {}
  virtual uint64_t HashFunction(const char*, uint32_t);
  virtual uint64_t MaxInputSize();
};

Hash* MakeHash(HashType hashType);

}  // namespace minikv

#endif  // INCLUDE_HASH_H_
