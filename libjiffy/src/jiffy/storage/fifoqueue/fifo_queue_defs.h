#ifndef JIFFY_FIFO_QUEUE_H
#define JIFFY_FIFO_QUEUE_H

#include <functional>
#include "jiffy/storage/block_memory_allocator.h"
#include "jiffy/storage/types/binary.h"
#include "string_array.h"
#include "string_array_persistent.h"

namespace jiffy {
namespace storage {

// Fifo queue definition
typedef string_array fifo_queue_type;
typedef string_array_persistent fifo_queue_persistent_type;

}
}

#endif //JIFFY_FIFO_QUEUE_H
