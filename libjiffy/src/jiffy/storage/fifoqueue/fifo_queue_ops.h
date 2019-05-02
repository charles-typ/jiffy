#ifndef JIFFY_FIFO_QUEUE_OPS_H
#define JIFFY_FIFO_QUEUE_OPS_H

#include <vector>
#include <string>
#include "jiffy/storage/command.h"

namespace jiffy {
namespace storage {
extern std::vector<command> FIFO_QUEUE_OPS;

/**
 * @brief FIFO queue supported operations
 */

enum fifo_queue_cmd_id {
  fq_enqueue = 0,
  fq_dequeue = 1,
  fq_clear = 2,
  fq_update_partition = 3,
};

}
}

#endif //JIFFY_FIFO_QUEUE_OPS_H