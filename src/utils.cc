#include "../include/utils.h"

#include <random>
#include <string>

namespace minikv {

// 生成随机字符串
std::string Utils::GenRandString(const uint16_t len) {
  std::string str;
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist6(0, 25);
  for (auto i = 0; i < len; i++) {
    str += 'a' + dist6(rng) % 26;
  }
  return str;
}

}  // namespace minikv