#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iterator>
#include <string>
#include <vector>

#include "../../include/database.h"
#include "../../include/file.h"
#include "../../include/log.h"
#include "../../include/record.h"
#include "../../include/utils.h"

int main() {
  minikv::DatabaseOptions database_option;
  minikv::Database db(database_option, "/tmp/minikv");

  minikv::Status s;

  std::string key;
  std::string val;
  std::string val_holder;

  minikv::GetOption get_option;
  minikv::PutOption put_option;

  auto loops = 10000000;

  for (auto i = 0; i < loops; i++) {
    key = minikv::Utils::GenRandString(10);
    val = minikv::Utils::GenRandString(10);
    s = db.Put(put_option, key, val);
    if (!s.IsOK()) std::cout << s.ToString() << std::endl;
    s = db.Get(get_option, key, &val_holder);
    if (!s.IsOK()) std::cout << s.ToString() << std::endl;
    if (val.compare(val_holder) != 0) {
      std::cout << "not equal" << std::endl;
    }
  }
}
