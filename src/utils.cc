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

// combine high 8 bit and low 56 bit to 64bit value,
// location ->  |  file number : 8 bit | offset inside file: 56 bit |
uint64_t Utils::GenLocation(const uint8_t file_num,
                            const uint64_t file_offset) {
  uint64_t location = file_num;
  location = location << 56;
  location += file_offset;
  return location;
}

uint8_t Utils::LocationDecodeFileNum(const uint64_t location) {
  return location >> 56;
}

uint64_t Utils::LocationDecodeFileOffset(const uint64_t location) {
  uint64_t mock_file_number = 1;
  mock_file_number = mock_file_number << 56;
  mock_file_number -= 1;
  return mock_file_number & location;
}

}  // namespace minikv