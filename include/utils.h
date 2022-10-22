#ifndef INCLUDE_UTILS_H_
#define INCLUDE_UTILS_H_

#include <cstdint>
#include <string>

namespace minikv {

class Utils {
 public:
  static std::string GenRandString(const uint16_t);
  static std::string MakeFixedLength(const uint32_t, const int);
  static uint64_t GenLocation(const uint32_t, const uint32_t);
  static uint32_t LocationDecodeFileNum(const uint64_t);
  static uint32_t LocationDecodeFileOffset(const uint64_t);
};

}  // namespace minikv

#endif  // INCLUDE_UTILS_H_
