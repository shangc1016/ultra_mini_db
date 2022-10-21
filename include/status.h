#ifndef INCLUDE_STATUS_H_
#define INCLUDE_STATUS_H_

#include <iostream>
#include <string>

namespace minikv {

enum StatusCode {
  STATUS_OKAY,
  STATUS_ERROR,
  STATUS_FILE_NOT_OPEN,
  STATUS_EMPTY_PTR,
  STATUS_NO_SPACE,
  STATUS_NOT_FOUND,
  STATUS_IO_ERROR,
  STATUS_BUFFER_FULL,
  STATUS_MMAP_FULL,
  STATUS_KEY_TOO_LONG,
  STATUS_VAL_TOO_LONG,
};

class Status {
 public:
  Status() : _statusCode(STATUS_OKAY), _statusMessage("") {}

  Status(StatusCode _code, std::string _msg)
      : _statusCode(_code), _statusMessage(_msg) {}

  ~Status() {}

  bool IsOK();

  std::string ToString();

 private:
  static std::string GetStatusCodeStr(StatusCode code);

  StatusCode _statusCode;
  std::string _statusMessage;
};

}  // namespace minikv

#endif  // INCLUDE_STATUS_H_
