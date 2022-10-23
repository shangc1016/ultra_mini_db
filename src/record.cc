#include "../include/record.h"

#include <cstdint>
#include <cstring>

namespace minikv {

uint32_t Record::GetRecordSize() {
  return sizeof(_is_deleted) + sizeof(_key_size) + sizeof(_val_size) +
         sizeof(_key_margin) + sizeof(_val_margin) +
         sizeof(char) * (_key_size + _val_size);
}

/*
record header layout:
    | _is_deleted | _key_size | _val_size | _key_margin | _val_margin |

    record layout
    | record header | fixed size _key | fixed size _val |
    | next record header   ....                         |

*/
Status Record::EncodeRecord(uint64_t address) {
  // step1, memcpy record header,
  memcpy((void *)address, &_is_deleted, sizeof(_is_deleted));
  address += sizeof(_is_deleted);
  memcpy((void *)address, &_key_size, sizeof(_key_size));
  address += sizeof(_key_size);
  memcpy((void *)address, &_val_size, sizeof(_val_size));
  address += sizeof(_val_size);
  memcpy((void *)address, &_key_margin, sizeof(_key_margin));
  address += sizeof(_key_margin);
  memcpy((void *)address, &_val_margin, sizeof(_val_margin));
  address += sizeof(_val_margin);
  memcpy((void *)address, _key.data(), sizeof(char) * _key_size);
  //   FIXME
  address += (_key_size + _key_margin);
  //  NOYE: memcpy std::vector
  //  std::copy to copy it out.
  memcpy((void *)address, _val.data(), sizeof(char) * _val_size);
  return Status(STATUS_OKAY, "Record::EncodeRecord");
}

Status Record::DecodeRecord(uint64_t address, Record &record) {
  memcpy(&record._is_deleted, (void *)address, sizeof(record._is_deleted));
  address += sizeof(record._is_deleted);
  memcpy(&record._key_size, (void *)address, sizeof(record._key_size));
  address += sizeof(record._key_size);
  memcpy(&record._val_size, (void *)address, sizeof(record._val_size));
  address += sizeof(record._val_size);
  memcpy(&record._key_margin, (void *)address, sizeof(record._key_margin));
  address += sizeof(record._key_margin);
  memcpy(&record._val_margin, (void *)address, sizeof(record._val_margin));
  address += sizeof(record._val_margin);
  std::copy((char *)address, (char *)address + record._key_size,
            std::back_inserter(record._key));
  //   FIXME
  address += (record._key_size + record._key_margin);
  std::copy((char *)address, (char *)address + record._val_size,
            std::back_inserter(record._val));
  return Status(STATUS_OKAY, "Record::DecodeRecord");
}

}  // namespace minikv