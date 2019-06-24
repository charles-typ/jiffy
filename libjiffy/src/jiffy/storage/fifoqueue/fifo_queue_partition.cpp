#include <jiffy/utils/string_utils.h>
#include "fifo_queue_partition.h"
#include "jiffy/storage/client/replica_chain_client.h"
#include "jiffy/utils/logger.h"
#include "jiffy/persistent/persistent_store.h"
#include "jiffy/storage/partition_manager.h"
#include "jiffy/storage/fifoqueue/fifo_queue_ops.h"
#include "jiffy/directory/client/directory_client.h"
#include "jiffy/auto_scaling/auto_scaling_client.h"
#include <thread>

namespace jiffy {
namespace storage {

using namespace utils;

fifo_queue_partition::fifo_queue_partition(block_memory_manager *manager,
                                           const std::string &name,
                                           const std::string &metadata,
                                           const utils::property_map &conf,
                                           const std::string &directory_host,
                                           int directory_port,
                                           const std::string &auto_scaling_host,
                                           int auto_scaling_port)
    : data_structure_partition<fifo_queue_type, size_t, block_memory_allocator<char>>(manager, name, metadata, directory_host, directory_port, auto_scaling_host, auto_scaling_port, FIFO_QUEUE_OPS, manager->mb_capacity(), build_allocator<char>()),
      new_block_available_(false) {
  auto ser = conf.get("fifoqueue.serializer", "csv");
  if (ser == "binary") {
    ser_ = std::make_shared<csv_serde>(binary_allocator_);
  } else if (ser == "csv") {
    ser_ = std::make_shared<binary_serde>(binary_allocator_);
  } else {
    throw std::invalid_argument("No such serializer/deserializer " + ser);
  }
  head_ = 0;
  threshold_hi_ = conf.get_as<double>("fifoqueue.capacity_threshold_hi", 0.95);
  auto_scale_ = conf.get_as<bool>("fifoqueue.auto_scale", true);
}

std::string fifo_queue_partition::enqueue(const std::string &message) {
  if (partition_.size() > partition_.capacity() - metadata_length) {
    if (!next_target_str().empty()) {
      new_block_available_ = true;
      return "!full!" + next_target_str();
    } else if(!auto_scale_) {
        return "!next_partition";
    }
    return "!redo";
  }
  auto ret = partition_.push_back(message);
  if (!ret.first) {
    new_block_available_ = true;
    if(!auto_scale_) {
      return "!split_enqueue!" + std::to_string(ret.second.size());
    }
    return "!split_enqueue!" + next_target_str() + "!" + std::to_string(ret.second.size());
  }
  return "!ok";
}

std::string fifo_queue_partition::dequeue() {
  auto ret = partition_.at(head_);
  if (ret.first) {
    head_ += (metadata_length + ret.second.size());
    return ret.second;
  } else if (ret.second == "!reach_end") {
    if (!next_target_str().empty()) {
      new_block_available_ = true;
      return "!msg_not_in_partition!" + next_target_str();
    } else if(!auto_scale_) {
      return "!next_partition";
    } else {
      return "!redo";
    }
  } else if (ret.second == "!not_available") {
    return "!msg_not_found";
  } else {
    head_ += (metadata_length + ret.second.size());
    if(!auto_scale_) {
      return "!split_dequeue!" + ret.second;
    }
    return "!split_dequeue!" + next_target_str() + "!" + ret.second;
  }
}

std::string fifo_queue_partition::readnext(std::string pos) {
  auto ret = partition_.at(std::stoi(pos));
  if (ret.first) {
    return ret.second;
  } else if (ret.second == "!reach_end") {
    if (!next_target_str().empty()) {
      new_block_available_ = true;
      return "!msg_not_in_partition" + next_target_str();
    } else if (!auto_scale_) {
      return "!next_partition";
    } else {
      return "!redo";
    }
  } else if (ret.second == "!not_available") {
    return "!msg_not_found";
  } else {
    if(!auto_scale_) {
      return "!split_readnext!" + ret.second;
    }
    return "!split_readnext!" + next_target_str() + "!" + ret.second;
  }
}

std::string fifo_queue_partition::clear() {
  partition_.clear();
  head_ = 0;
  overload_ = false;
  underload_ = false;
  dirty_ = false;
  return "!ok";
}

std::string fifo_queue_partition::update_partition(const std::string &next) {
  next_target(next);
  return "!ok";
}

void fifo_queue_partition::run_command(std::vector<std::string> &_return,
                                       int32_t cmd_id,
                                       const std::vector<std::string> &args) {
  size_t nargs = args.size();
  switch (cmd_id) {
    case fifo_queue_cmd_id::fq_enqueue:
      for (const std::string &msg: args)
        _return.emplace_back(enqueue(msg));
      break;
    case fifo_queue_cmd_id::fq_dequeue:
      if (nargs != 0) {
        _return.emplace_back("!args_error");
      } else {
        _return.emplace_back(dequeue());
      }
      break;
    case fifo_queue_cmd_id::fq_clear:
      if (nargs != 0) {
        _return.emplace_back("!args_error");
      } else {
        _return.emplace_back(clear());
      }
      break;
    case fifo_queue_cmd_id::fq_update_partition:
      if (nargs != 1) {
        _return.emplace_back("!args_error");
      } else {
        _return.emplace_back(update_partition(args[0]));
      }
      break;
    case fifo_queue_cmd_id::fq_readnext:
      if (nargs != 1) {
        _return.emplace_back("!args_error");
      } else {
        _return.emplace_back(readnext(args[0]));
      }
      break;
    default:throw std::invalid_argument("No such operation id " + std::to_string(cmd_id));
  }
  if (is_mutator(cmd_id)) {
    dirty_ = true;
  }
  if (auto_scale_ && is_mutator(cmd_id) && overload() && is_tail() && !overload_) {
    LOG(log_level::info) << "Overloaded partition; storage = " << storage_size() << " capacity = "
                         << storage_capacity() << " partition size = " << size() << "partition capacity "
                         << partition_.capacity();
    try {
      overload_ = true;
      std::string dst_partition_name = std::to_string(std::stoi(name_) + 1);
      std::map<std::string, std::string> scale_conf;
      scale_conf.emplace(std::make_pair(std::string("type"), std::string("fifo_queue_add")));
      scale_conf.emplace(std::make_pair(std::string("next_partition_name"), dst_partition_name));
      auto scale = std::make_shared<auto_scaling::auto_scaling_client>(auto_scaling_host_, auto_scaling_port_);
      scale->auto_scaling(chain(), path(), scale_conf);
    } catch (std::exception &e) {
      overload_ = false;
      LOG(log_level::warn) << "Adding new fifo queue partition failed: " << e.what();
    }
  }
  if (auto_scale_ && cmd_id == fifo_queue_cmd_id::fq_dequeue && head_ > partition_.capacity() && is_tail()
      && !underload_ && new_block_available_ == true) {
    try {
      LOG(log_level::info) << "Underloaded partition; storage = " << storage_size() << " capacity = "
                           << storage_capacity() << " partition size = " << size() << "partition capacity "
                           << partition_.capacity();
      underload_ = true;
      std::string dst_partition_name = std::to_string(std::stoi(name_) + 1);
      std::map<std::string, std::string> scale_conf;
      scale_conf.emplace(std::make_pair(std::string("type"), std::string("fifo_queue_delete")));
      scale_conf.emplace(std::make_pair(std::string("current_partition_name"), name()));
      auto scale = std::make_shared<auto_scaling::auto_scaling_client>(auto_scaling_host_, auto_scaling_port_);
      scale->auto_scaling(chain(), path(), scale_conf);
    } catch (std::exception &e) {
      underload_ = false;
      LOG(log_level::warn) << "Removing fifo queue partition failed: " << e.what();
    }
  }
}

void fifo_queue_partition::clear_all() {
  partition_.clear();
  next_->reset("nil");
  path_ = "";
  sub_map_.clear();
  chain_ = {};
  role_ = singleton;
  overload_ = false;
  dirty_ = false;
}

void fifo_queue_partition::forward_all() {
  int64_t i = 0;
  for (auto it = partition_.begin(); it != partition_.end(); it++) {
    std::vector<std::string> result;
    run_command_on_next(result, fifo_queue_cmd_id::fq_enqueue, {*it});
    ++i;
  }
}

REGISTER_IMPLEMENTATION("fifoqueue", fifo_queue_partition);

}
}