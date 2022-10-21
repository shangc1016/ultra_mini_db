#ifndef __UTILS_H_
#define __UTILS_H_

#include <cstdint>
#include <string>

namespace minikv {

class Utils {
 public:
  static std::string GenRandString(const uint16_t);
};

}  // namespace minikv

#endif