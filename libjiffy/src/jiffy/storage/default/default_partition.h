#ifndef JIFFY_DEFAULT_SERVICE_SHARD_H
#define JIFFY_DEFAULT_SERVICE_SHARD_H

#include <string>
#include <jiffy/utils/property_map.h>
#include "jiffy/storage/partition.h"
#include "jiffy/storage/data_structure_partition.h"

namespace jiffy {
namespace storage {

/**
 * Default partition class
 * When partition is deleted, change to default partition and ends connection
 */
class default_partition : public data_structure_partition {
 public:

  /**
   * @brief Constructor
   * @param manager Block memory manager
   * @param name Partition name
   * @param metadata Partition metadata
   * @param conf Partition property map
   * @param directory_host Directory server host name
   * @param directory_port Directory server port number
   * @param auto_scaling_host Auto scaling server host name
   * @param auto_scaling_port Auto scaling server port number
   */
  explicit default_partition(block_memory_manager *manager,
                               const std::string &name = "default",
                               const std::string &metadata = "default",
                               const utils::property_map &conf = {},
                               const std::string &directory_host = "localhost",
                               int directory_port = 9091,
                               const std::string &auto_scaling_host = "localhost",
                               int auto_scaling_port = 9095);

  /**
   * @brief Virtual destructor
   */
  virtual ~default_partition() = default;

  /**
   * @brief Run particular command on key value block
   * @param _return Return status to be collected
   * @param cmd_id Operation identifier
   * @param args Command arguments
   */

  void run_command(std::vector<std::string> &_return, int cmd_id, const std::vector<std::string> &args) override;
  /**
   * @brief Load persistent data into the block, lock the block while doing this
   * @param path Persistent storage path
   */
  void load(const std::string &path) override;
  /**
   * @brief If dirty, synchronize persistent storage and block
   * @param path Persistent storage path
   * @return Bool value, true if block successfully synchronized
   */

  bool sync(const std::string &path) override;

  /**
   * @brief Flush the block if dirty and clear the block
   * @param path Persistent storage path
   * @return Bool value, true if block successfully dumped
   */

  bool dump(const std::string &path) override;

  /**
   * @brief Clear all content in the partition
   */
  std::string clear() override;
  
  /**
   * @brief Send all key and value to the next block
   */

  void forward_all() override;
};
}
}

#endif //JIFFY_DEFAULT_SERVICE_SHARD_H
