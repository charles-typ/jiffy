#ifndef JIFFY_HASH_TABLE_OPS_H
#define JIFFY_HASH_TABLE_OPS_H

#include <vector>
#include "jiffy/storage/command.h"

namespace jiffy {
namespace storage {

extern std::vector<command> KV_OPS;

/**
 * @brief Hash table supported operations
 */
enum class hash_table_cmd_id {
  exists = 0,
  get = 1,
  keys = 2, // TODO: We should not support multi-key operations since we do not provide any guarantees
  num_keys = 3, // TODO: We should not support multi-key operations since we do not provide any guarantees
  put = 4,
  remove = 5,
  update = 6,
  lock = 7,
  unlock = 8,
  locked_data_in_slot_range = 9,
  locked_get = 10,
  locked_put = 11,
  locked_remove = 12,
  locked_update = 13,
  upsert = 14,
  locked_upsert = 15,
  update_partition = 16,
  locked_update_partition = 17,
  get_storage_size = 18,
  locked_get_storage_size = 19,
  get_metadata = 20,
  locked_get_metadata = 21
};

}
}

#endif //JIFFY_HASH_TABLE_OPS_H
