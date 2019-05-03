#ifndef JIFFY_FILE_H
#define JIFFY_FILE_H

#include <functional>
#include <deque>
#include "jiffy/storage/block_memory_allocator.h"
#include "jiffy/storage/types/binary.h"
#include "monolog_linear.h"

namespace jiffy {
namespace storage {

// The default number of elements in a file
constexpr size_t FILE_DEFAULT_SIZE = 0;

// Message definitions
typedef binary msg_type;

// Custom template arguments
typedef block_memory_allocator<msg_type> file_allocator_type;

// Msg queue definitions
typedef std::vector<msg_type, file_allocator_type> file_type;




/**
 * Constants for the data log
 */
class data_log_constants {
 public:
  /** Maximum number of blocks */
  static const size_t MAX_BUCKETS = 65536;
  /** The size of each block */
  static const size_t BUCKET_SIZE = 67108864;
  /** The size of the buffer */
  static const size_t BUFFER_SIZE = 1048576;
};

typedef confluo::monolog::monolog_linear<uint8_t,
                                         data_log_constants::MAX_BUCKETS,
                                         data_log_constants::BUCKET_SIZE,
                                         data_log_constants::BUFFER_SIZE> file_type_2;

}
}

#endif //JIFFY_FILE_H
