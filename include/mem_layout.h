#ifndef INCLUDE_MEM_LAYOUT_H_
#define INCLUDE_MEM_LAYOUT_H_

#include <cstdint>
#include <map>
#include <vector>

#include "./status.h"

namespace minikv {

class IndexHeader {
 public:
  static Status EncodeIndexHeader(uint64_t, const IndexHeader &);
  static Status DecodeIndexHeader(uint64_t, IndexHeader &);
  static void Print(const IndexHeader &);

  IndexHeader(uint64_t size, uint64_t nums) : _version(0), _checksum(0) {
    // not concert first two.
    _index_size = size;
    _index_nums = nums;
  }

  uint64_t _version;
  uint64_t _checksum;
  uint64_t _index_size;
  uint64_t _index_nums;
};

class RecordHeader {
 public:
  static Status EncodeRecordHeader(uint64_t, const RecordHeader &);
  static Status DecodeRecordHeader(uint64_t, RecordHeader &);
  static void Print(const RecordHeader &);

  RecordHeader(uint64_t ioffset, uint64_t roffset, uint64_t isize,
               uint64_t inums)
      : _version(0), _check_sum(0) {
    _index_offset = ioffset;
    _record_offset = roffset;
    _index_item_size = isize;
    _index_item_nums = inums;
    _kv_item_nums = inums;
  }

  uint64_t _version;
  uint64_t _check_sum;
  uint64_t _kv_item_nums;
  uint64_t _index_offset;
  uint64_t _record_offset;
  uint64_t _index_item_size;
  uint64_t _index_item_nums;
};

Status EncodeIndexVec(uint64_t, const std::multimap<uint64_t, uint64_t> &);
Status DecodeIndexVec(uint64_t, uint64_t, std::multimap<uint64_t, uint64_t> &);
void PrintIndexVec(const std::multimap<uint64_t, uint64_t> &);

class Record {
 public:
  // static function
  static Status EncodeRecord(uint64_t, const Record &);
  static Status DecodeRecord(uint64_t, Record &);
  static void Print(const Record &, bool);
  static uint32_t RecordSize(const Record &);
  // none static function
  // uint32_t GetRecordSize();

  Record(const std::string &key, const std::string &val, bool is_delete,
         uint64_t max_key_sz, uint64_t max_val_sz) {
    _is_deleted = is_delete;
    _key = std::vector<char>(key.begin(), key.end());
    _key.push_back('\0');
    _val = std::vector<char>(val.begin(), val.end());
    _val.push_back('\0');
    _key_size = _key.size();
    _val_size = _val.size();
    _key_margin = max_key_sz - _key_size;
    _val_margin = max_val_sz - _val_size;
  }

  Record() {}

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

}  // namespace minikv
#endif  // INCLUDE_MEM_LAYOUT_H_
