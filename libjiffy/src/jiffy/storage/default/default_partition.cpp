#include <jiffy/utils/string_utils.h>
#include "default_partition.h"
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

default_partition::default_partition(block_memory_manager *manager,
                                           const std::string &name,
                                           const std::string &metadata,
                                           const utils::property_map &conf,
                                           const std::string &directory_host,
                                           const int directory_port,
                                           const std::string &auto_scaling_host,
                                           const int auto_scaling_port)
    : chain_module(manager, name, metadata, {}){}

void default_partition::run_command(std::vector<std::string> &_return,
                                       int32_t cmd_id,
                                       const std::vector<std::string> &args) {
      LOG(log_level::info) << "Into default partition function";
      _return.emplace_back("!block_moved");
}

void default_partition::load(const std::string &path) {
}

bool default_partition::sync(const std::string &path) {
}

bool default_partition::dump(const std::string &path) {
}

void default_partition::forward_all() {
}


REGISTER_IMPLEMENTATION("default", default_partition);

}
}