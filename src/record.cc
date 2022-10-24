#include "../include/record.h"

#include <cstdint>
#include <cstring>

namespace minikv {

uint32_t Record::GetRecordSize() {
  // BUG: remember the critical record mmap layout.
  // !!!!!!!
  return sizeof(_is_deleted) + sizeof(_key_size) + sizeof(_val_size) +
         sizeof(_key_margin) + sizeof(_val_margin) + _key_size + _key_margin +
         _val_size + _val_margin;
}

/*
record header layout:
    | _is_deleted | _key_size | _val_size | _key_margin | _val_margin |

    record layout
    | record header | fixed size _key | fixed size _val |
    | next record header   ....                         |

*/
Status Record::EncodeRecord(uint64_t address, const Record &record) {
  // step1, memcpy record header.
  memcpy((void *)address, &record._is_deleted, sizeof(_is_deleted));
  address += sizeof(_is_deleted);

  memcpy((void *)address, &record._key_size, sizeof(_key_size));
  address += sizeof(_key_size);

  memcpy((void *)address, &record._val_size, sizeof(_val_size));
  address += sizeof(_val_size);

  memcpy((void *)address, &record._key_margin, sizeof(_key_margin));
  address += sizeof(_key_margin);

  memcpy((void *)address, &record._val_margin, sizeof(_val_margin));
  address += sizeof(_val_margin);

  // step2: memcpy key and val.
  // fprintf(stdout, "[encode]: key addr = 0x%lx\n", address);
  memcpy((void *)address, record._key.data(), sizeof(char) * record._key_size);
  address += (record._key_size + record._key_margin);

  memcpy((void *)address, record._val.data(), sizeof(char) * record._val_size);

  return Status(STATUS_OKAY, "Record::EncodeRecord");
}

Status Record::DecodeRecord(uint64_t address, Record &record) {
  // step1: memcpy record header.
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

  // step2: memcpy key and val.
  // fprintf(stdout, "[decode]: key addr = 0x%lx\n", address);
  std::copy((char *)address, (char *)address + record._key_size,
            std::back_inserter(record._key));
  address += (record._key_size + record._key_margin);

  std::copy((char *)address, (char *)address + record._val_size,
            std::back_inserter(record._val));

  return Status(STATUS_OKAY, "Record::DecodeRecord");
}

void Record::Print(const Record &record, bool full) {
  if (full) {
    fprintf(stdout, "[delete]: %d\t", record._is_deleted);

    fprintf(stdout, "[k_size]: %ld\t", record._key_size);
    fprintf(stdout, "[v_size]: %ld\t", record._val_size);

    fprintf(stdout, "[k_merg]: %ld\t", record._key_margin);
    fprintf(stdout, "[v_merg]: %ld\t", record._val_margin);

    fprintf(stdout, "[k]: %s\t", record._key.data());
    fprintf(stdout, "[v]:%s\n", record._val.data());
  } else {
    fprintf(stdout, "[k]: %s\t", record._key.data());
    fprintf(stdout, "[v]: %s\n", record._val.data());
  }
}

}  // namespace minikv