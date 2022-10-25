#include "../include/database.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <memory>

#include "../include/eventmanager.h"

namespace minikv {

// 设置相关参数
Database::Database(const DatabaseOptions &db_option, std::string db_path) {
  _db_options = db_option;
  // db_path会覆盖db_option._db_path
  if (!db_path.empty()) _db_options._db_path = db_path;

  // for buffer sync between wb and se.
  // FIXBUG: 刚开始的时候，这儿是栈上分配的，离开作用域直接内存释放了，
  // 下面两个实例化对象用不了。傻了。
  _flush_event = new Event<std::vector<Record>>();
  // wb
  _write_buffer = new WriteBuffer(_db_options, _flush_event);
  // se
  _storage_engine = new StorageEngine(_db_options, _flush_event);
}

Database::~Database() {
  // stop thread
  _write_buffer->Stop();
  _storage_engine->Stop();

  // delete _write_buffer;
  // delete _storage_engine;
}

Status Database::Open() {
  // _dbName是一个目录，需要在这个目录下面创建多个存文件;
  // 检查路径是否存在，检查路径是否是个文件

  // 创建database/db_option文件
  // 如果db_option文件存在，从其中读取配置选项,,,使用mmap的方式，把db_option文件映射到内存
  // 否则从HATable中得到db_option，
  // 如果啥都没有，就把初始化用到的db_option写到文件中

  // 初始化哈希函数，

  // 实例化StorageEngine
  // 实例化WriteBuffer
  // 需要把线程同步的一些参数传递给两个对象

  return Status(STATUS_OKAY, "Open Database successed.");
}

Status Database::Get(const std::string &key, std::string &value) {
  GetOption get_option;
  // FIXME-01
  return Get(get_option, key, value);
}

Status Database::Get(GetOption &get_option, const std::string &key,
                     std::string &value) {
  // first: search in `wb`.
  auto s = _write_buffer->Get(get_option, key, value);
  if (s.IsOK()) return s;
  // otherwise, search in `se`.
  return _storage_engine->Get(get_option, key, value);
}

Status Database::Put(const std::string &key, const std::string &value) {
  PutOption put_option;
  return Put(put_option, key, value);
}

Status Database::Put(PutOption &put_option, const std::string &key,
                     const std::string &value) {
  return _write_buffer->Put(put_option, key, value);
}

Status Database::Delete(const std::string &key) {
  PutOption put_option;
  return _write_buffer->Delete(put_option, key);
}
}  // namespace minikv
