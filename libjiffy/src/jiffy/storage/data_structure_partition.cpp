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
template<class T, class... Rest>
data_structure_partition<T, Rest...>::data_structure_partition(block_memory_manager *manager,
                                     const std::string &name,
                                     const std::string &metadata,
                                     const std::string &directory_host,
                                     int directory_port,
                                     const std::string &auto_scaling_host,
                                     int auto_scaling_port,
                                     const std::vector<command> &supported_cmds,
                                     Rest... values)
    : chain_module(manager, name, metadata, supported_cmds),
      partition_(std::forward<Rest>(values)...),
      overload_(false),
      underload_(false),
      dirty_(false),
      directory_host_(directory_host),
      directory_port_(directory_port),
      auto_scaling_host_(auto_scaling_host),
      auto_scaling_port_(auto_scaling_port) {
}
template<class T, class... Rest>
bool data_structure_partition<T, Rest...>::is_dirty() const {
  return dirty_;
}

template<class T, class... Rest>
void data_structure_partition<T, Rest...>::load(const std::string &path) {
  auto remote = persistent::persistent_store::instance(path, ser_);
  auto decomposed = persistent::persistent_store::decompose_path(path);
  remote->read<T>(decomposed.second, partition_);
}

template<class T, class... Rest>
bool data_structure_partition<T, Rest...>::sync(const std::string &path) {
  if (dirty_) {
    auto remote = persistent::persistent_store::instance(path, ser_);
    auto decomposed = persistent::persistent_store::decompose_path(path);
    remote->write<T>(partition_, decomposed.second);
    dirty_ = false;
    return true;
  }
  return false;
}

template<class T, class... Rest>
bool data_structure_partition<T, Rest...>::dump(const std::string &path) {
  bool flushed = false;
  if (dirty_) {
    auto remote = persistent::persistent_store::instance(path, ser_);
    auto decomposed = persistent::persistent_store::decompose_path(path);
    remote->write<T>(partition_, decomposed.second);
    flushed = true;
  }
  clear_all();
  return flushed;
}
template<class T, class... Rest>
std::size_t data_structure_partition<T, Rest...>::size() const {
  return partition_.size();
}
template<class T, class... Rest>
bool data_structure_partition<T, Rest...>::empty() const {
  return partition_.empty();
}


template class data_structure_partition<hash_table_type, size_t, hash_type, equal_type>;
template class data_structure_partition<fifo_queue_type, size_t, block_memory_allocator<char>>;
template class data_structure_partition<file_type, size_t, block_memory_allocator<char>>;
}
}