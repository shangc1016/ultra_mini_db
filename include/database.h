#ifndef INCLUDE_DATABASE_H_
#define INCLUDE_DATABASE_H_

#include <string>
#include <vector>

#include "eventmanager.h"
#include "options.h"
#include "status.h"
#include "storageengine.h"
#include "writebuffer.h"

namespace minikv {

class Database {
 public:
  Database(const DatabaseOptions &, std::string);
  ~Database();

  // forbid assign operator
  Database &operator=(const Database &) = delete;
  // forbid copying constructor
  Database(const Database &) = delete;

  Status Open();

  Status Get(const std::string &, std::string &);
  Status Get(GetOption &, const std::string &, std::string &);

  Status Put(const std::string &, const std::string &);
  Status Put(PutOption &, const std::string &, const std::string &);

  Status Delete(const std::string &);

  // TODO(shang): additerator to traverse entire db.
  Status NewIterator();
  Status Compact();  // 压缩db空间
  Status Flush();    // Flush数据库的数据到磁盘'

 private:
  int _fd;
  bool _is_open;  // 为啥有这个参数?

  DatabaseOptions _db_options;
  StorageEngine *_storage_engine;
  WriteBuffer *_write_buffer;

  Event<std::vector<Record>> *_flush_event;

  // 压缩
  // 校验
  // 线程
};

}  // namespace minikv

#endif  // INCLUDE_DATABASE_H_
