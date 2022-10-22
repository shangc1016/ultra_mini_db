#include "../include/utils.h"

#include <cstdint>
#include <iomanip>
#include <random>
#include <sstream>
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

std::string Utils::MakeFixedLength(const uint32_t num, const int length) {
  std::ostringstream ostr;
  if (num < 0) ostr << "-";
  ostr << std::setfill('0') << std::setw(length) << (num < 0 ? -num : num);
  return ostr.str();
}

// combine high 32 bit and low 32 bit to 64bit value,
// location ->  |  file number : 32bit | offset inside file: 23bit |
uint64_t Utils::GenLocation(const uint32_t high, const uint32_t low) {
  uint64_t value = high;
  value = value << 32;
  value += low;
  return value;
}

uint32_t Utils::LocationDecodeFileNum(const uint64_t location) {
  return (location - LocationDecodeFileOffset(location)) >> 32;
}

uint32_t Utils::LocationDecodeFileOffset(const uint64_t location) {
  return location;
}

}  // namespace minikv