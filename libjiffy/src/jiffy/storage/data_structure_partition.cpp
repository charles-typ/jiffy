#include <jiffy/utils/string_utils.h>
#include "data_structure_partition.h"
#include "jiffy/storage/client/replica_chain_client.h"
#include "jiffy/utils/logger.h"
#include "jiffy/persistent/persistent_store.h"
#include "jiffy/storage/partition_manager.h"
#include "jiffy/directory/client/directory_client.h"
#include "jiffy/auto_scaling/auto_scaling_client.h"
#include <thread>

namespace jiffy {
namespace storage {

using namespace utils;

data_structure_partition::data_structure_partition(block_memory_manager *manager,
                                     const std::string &name,
                                     const std::string &metadata,
                                     const utils::property_map &conf,
                                     const std::string &directory_host,
                                     int directory_port,
                                     const std::string &auto_scaling_host,
                                     int auto_scaling_port,
                                     const std::vector<command> &supported_cmds)
    : chain_module(manager, name, metadata, supported_cmds),
      overload_(false),
      underload_(false),
      dirty_(false),
      directory_host_(directory_host),
      directory_port_(directory_port),
      auto_scaling_host_(auto_scaling_host),
      auto_scaling_port_(auto_scaling_port) {
}

bool data_structure_partition::overload() {
  return storage_size() > static_cast<size_t>(static_cast<double>(storage_capacity()) * threshold_hi_);
}

bool data_structure_partition::underload() {
  return storage_size() < static_cast<size_t>(static_cast<double>(storage_capacity()) * threshold_lo_);
}
}
}