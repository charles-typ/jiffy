#ifndef JIFFY_DATA_STRUCTURE_PARTITION_SHARD_H
#define JIFFY_DATA_STRUCTURE_PARTITION_SHARD_H

#include <string>
#include <jiffy/utils/property_map.h>
#include "jiffy/storage/partition.h"
#include "jiffy/storage/chain_module.h"
#include "jiffy/storage/serde/serde_all.h"

namespace jiffy {
namespace storage {

class data_structure_partition : public chain_module {
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
  explicit data_structure_partition(block_memory_manager *manager,
                               const std::string &name,
                               const std::string &metadata,
                               const utils::property_map &conf,
                               const std::string &directory_host,
                               int directory_port,
                               const std::string &auto_scaling_host,
                               int auto_scaling_port,
                               const std::vector<command> &supported_cmds);

  /**
   * @brief Virtual destructor
   */
  virtual ~data_structure_partition() = default;
  
   /**
   * Management Operations
   * Virtual function
   */

  virtual void load(const std::string &path) = 0;

  /**
   * @brief Synchronize partition with persistent store.
   * @param path Persistent store path to write to.
   * @return True if data was written, false otherwise.
   */
  virtual bool sync(const std::string &path) = 0;

  /**
   * @brief Dump partition data to persistent store.
   * @param path Persistent store path to write to.
   * @return True if data was written, false otherwise.
   */
  virtual bool dump(const std::string &path) = 0;

  /**
   * @brief Virtual function for forwarding all
   */

  virtual void forward_all() = 0;

 protected:

  /**
   * @brief Check if block is overloaded
   * @return Bool value, true if block size is over the high threshold capacity
   */

  bool overload();

  /**
   * @brief Check if block is underloaded
   * @return Bool value, true if block size is under the low threshold capacity
   */

  bool underload();

  /* Custom serializer/deserializer */
  std::shared_ptr<serde> ser_;

  /* Low threshold */
  double threshold_lo_;
  /* High threshold */
  double threshold_hi_;

  /* Bool for partition hash slot range splitting */
  bool overload_;

  /* Bool for partition hash slot range merging */
  bool underload_;

  /* Bool partition dirty bit */
  bool dirty_;

  /* Bool value for auto scaling */
  bool auto_scale_;

  /* Directory server hostname */
  std::string directory_host_;

  /* Directory server port number */
  int directory_port_;

  /* Auto scaling server hostname */
  std::string auto_scaling_host_;

  /* Auto scaling server port number */
  int auto_scaling_port_;

};
}
}

#endif //JIFFY_DATA_STRUCTURE_PARTITION_SHARD_H
