#include "../../include/database.h"
#include "../../include/log.h"
#include "../../include/utils.h"

using namespace minikv;

int main() {
  minikv::Logger::SetLogLevel(minikv::LogTrace);

  DatabaseOptions db_options;
  Database db(db_options, "/tmp/minikv");

  std::string key;
  std::string val;
  std::string value_place_holder;

  int loops = 1000;

  for (auto i = 0; i < loops; i++) {
    key = Utils::GenRandString(10);
    val = Utils::GenRandString(10);

    auto status = db.Put(key, val);
    if (!status.IsOK()) std::cout << status.ToString() << std::endl;

    status = db.Get(key, &value_place_holder);
    if (!status.IsOK()) std::cout << status.ToString() << std::endl;

    if (val.compare(value_place_holder) != 0) {
      std::cout << "[not equal]" << std::endl;
    }
  }
}
