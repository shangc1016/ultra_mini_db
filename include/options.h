#ifndef INCLUDE_OPTIONS_H_
#define INCLUDE_OPTIONS_H_

#include <cstdint>
#include <string>

namespace minikv {

// 使用的哈希算法
enum HashType {

  CityHash64,
  MurmurHash128,

};

// PUT的参数,可以选择同步或者异步
struct GetOption {
  bool get;
};

// GET的参数:可以选择检查校验和
struct PutOption {
  bool put;
};

// 包括了文件类型
struct DatabaseOptions {
  // 在持久化的时候，hstable的大小
  uint64_t _internal_hstable_header_sz;

  // 哈希函数的选择
  HashType _hash_type;

  // writeBuffer中两个buffer的最大size，超过最大size就需要flush；
  uint64_t _max_wb_buffer_size;

  // 在写入数据的时候需不需要压缩
  bool _need_compress;

  // 创建数据库的时候，不需要数据库目录提前准备好
  bool _create_db_dir_if_not_exist;

  std::string _db_path;

  int _max_single_file_size;

  // key的最大长度
  uint64_t _max_key_size;
  // value的最大长度
  uint64_t _max_val_size;

  DatabaseOptions() {
    // 1KB
    _internal_hstable_header_sz = 0x400;

    _hash_type = CityHash64;

    _max_wb_buffer_size = 100;

    _need_compress = false;

    _create_db_dir_if_not_exist = true;

    _db_path = "/tmp/minikv";

    //  1GB
    _max_single_file_size = (1 << 20);

    _max_key_size = 32;

    _max_val_size = 32;
  }
};

}  // namespace minikv

#endif  // INCLUDE_OPTIONS_H_
