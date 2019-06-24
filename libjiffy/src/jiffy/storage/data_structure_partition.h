#ifndef JIFFY_DATA_STRUCTURE_PARTITION_SHARD_H
#define JIFFY_DATA_STRUCTURE_PARTITION_SHARD_H

#include <string>
#include <jiffy/utils/property_map.h>
#include "jiffy/storage/partition.h"
#include "jiffy/storage/chain_module.h"
#include "jiffy/storage/serde/serde_all.h"
#include "jiffy/storage/block_memory_allocator.h"

namespace jiffy {
namespace storage {

template<class T, class... Rest>
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
                               const std::string &directory_host,
                               int directory_port,
                               const std::string &auto_scaling_host,
                               int auto_scaling_port,
                               const std::vector<command> &supported_cmds,
                               Rest... values);

  /**
   * @brief Virtual destructor
   */
  virtual ~data_structure_partition() = default;

  virtual std::string clear() = 0;
  /**
   * @brief Check dirty bit
   * @return Bool value, true if block is dirty
   */
  bool is_dirty() const;

   /**
   * Management Operations
   * Virtual function
   */

  void load(const std::string &path) override;

  /**
   * @brief Synchronize partition with persistent store.
   * @param path Persistent store path to write to.
   * @return True if data was written, false otherwise.
   */
  bool sync(const std::string &path) override;

  /**
   * @brief Dump partition data to persistent store.
   * @param path Persistent store path to write to.
   * @return True if data was written, false otherwise.
   */
  bool dump(const std::string &path) override;

  /**
   * @brief Virtual function for forwarding all
   */

  virtual void forward_all() = 0;

  /**
   * @brief Clear all data and metadata of partition
   */
  virtual void clear_all() = 0;

  /**
   * @brief Fetch block size
   * @return Block size
   */

  std::size_t size() const;

  /**
   * @brief Check if block is empty
   * @return Bool value, true if empty
   */

  bool empty() const;

 protected:
  
  T partition_;
  /* Custom serializer/deserializer */
  std::shared_ptr<serde> ser_;

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
