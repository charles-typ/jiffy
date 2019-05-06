#include "hash_table_client.h"
#include "jiffy/utils/logger.h"
#include "jiffy/utils/string_utils.h"
#include "jiffy/storage/hashtable/hash_slot.h"
#include <thread>
#include <cmath>

namespace jiffy {
namespace storage {

using namespace jiffy::utils;

hash_table_client::hash_table_client(std::shared_ptr<directory::directory_interface> fs,
                                     const std::string &path,
                                     const directory::data_status &status,
                                     int timeout_ms)
    : data_structure_client(fs, path, status, KV_OPS, timeout_ms) {
  blocks_.clear();
  for (auto &block: status.data_blocks()) {
    blocks_.emplace(std::make_pair(static_cast<int32_t>(std::stoi(utils::string_utils::split(block.name, '_')[0])),
                                   std::make_shared<replica_chain_client>(fs_, path_, block, KV_OPS, timeout_ms_)));
  }
}

void hash_table_client::refresh() {
  status_ = fs_->dstatus(path_);
  blocks_.clear();
  for (auto &block: status_.data_blocks()) {
    if (block.metadata != "split_importing" && block.metadata != "importing") {
      blocks_.emplace(std::make_pair(static_cast<int32_t>(std::stoi(utils::string_utils::split(block.name, '_')[0])),
                                     std::make_shared<replica_chain_client>(fs_, path_, block, KV_OPS, timeout_ms_)));
    }
  }
}

std::string hash_table_client::put(const std::string &key, const std::string &value) {
  std::string _return;
  std::vector<std::string> args{key, value};
  bool redo;
  do {
    try {
      _return = blocks_[block_id(key)]->run_command(hash_table_cmd_id::ht_put, args).front();
      handle_redirect(hash_table_cmd_id::ht_put, args, _return);
      redo = false;
      redo_times = 0;
    } catch (redo_error &e) {
      redo = true;
    }
  } while (redo);
  return _return;
}

std::string hash_table_client::get(const std::string &key) {
  std::string _return;
  std::vector<std::string> args{key};
  bool redo;
  do {
    try {
      _return = blocks_[block_id(key)]->run_command(hash_table_cmd_id::ht_get, args).front();
      handle_redirect(hash_table_cmd_id::ht_get, args, _return);
      redo = false;
      redo_times = 0;
    } catch (redo_error &e) {
      redo = true;
    }
  } while (redo);
  return _return;
}

std::string hash_table_client::update(const std::string &key, const std::string &value) {
  std::string _return;
  std::vector<std::string> args{key, value};
  bool redo;
  do {
    try {
      _return = blocks_[block_id(key)]->run_command(hash_table_cmd_id::ht_update, args).front();
      handle_redirect(hash_table_cmd_id::ht_update, args, _return);
      redo = false;
      redo_times = 0;
    } catch (redo_error &e) {
      redo = true;
    }
  } while (redo);
  return _return;
}

std::string hash_table_client::remove(const std::string &key) {
  std::string _return;
  std::vector<std::string> args{key};
  bool redo;
  do {
    try {
      _return = blocks_[block_id(key)]->run_command(hash_table_cmd_id::ht_remove, args).front();
      //LOG(log_level::info) << "The return value is" << _return;
      handle_redirect(hash_table_cmd_id::ht_remove, args, _return);
      redo = false;
      redo_times = 0;
    } catch (redo_error &e) {
      redo = true;
    }
  } while (redo);
  return _return;
}

std::vector<std::string> hash_table_client::put(const std::vector<std::string> &kvs) {
  if (kvs.size() % 2 != 0) {
    throw std::invalid_argument("Incorrect number of arguments");
  }
  std::vector<std::string> _return;
  bool redo;
  do {
    try {
      _return = batch_command(hash_table_cmd_id::ht_put, kvs, 2);
      handle_redirects(hash_table_cmd_id::ht_put, kvs, _return);
      redo = false;
    } catch (redo_error &e) {
      redo = true;
    }
  } while (redo);
  return _return;
}

std::vector<std::string> hash_table_client::get(const std::vector<std::string> &keys) {
  std::vector<std::string> _return;
  bool redo;
  do {
    try {
      _return = batch_command(hash_table_cmd_id::ht_get, keys, 1);
      handle_redirects(hash_table_cmd_id::ht_get, keys, _return);
      redo = false;
    } catch (redo_error &e) {
      redo = true;
    }
  } while (redo);
  return _return;
}

std::vector<std::string> hash_table_client::update(const std::vector<std::string> &kvs) {
  if (kvs.size() % 2 != 0) {
    throw std::invalid_argument("Incorrect number of arguments");
  }
  std::vector<std::string> _return;
  bool redo;
  do {
    try {
      _return = batch_command(hash_table_cmd_id::ht_update, kvs, 2);
      handle_redirects(hash_table_cmd_id::ht_update, kvs, _return);
      redo = false;
    } catch (redo_error &e) {
      redo = true;
    }
  } while (redo);
  return _return;
}

std::vector<std::string> hash_table_client::remove(const std::vector<std::string> &keys) {
  std::vector<std::string> _return;
  bool redo;
  do {
    try {
      _return = batch_command(hash_table_cmd_id::ht_remove, keys, 1);
      handle_redirects(hash_table_cmd_id::ht_remove, keys, _return);
      redo = false;
    } catch (redo_error &e) {
      redo = true;
    }
  } while (redo);
  return _return;
}

std::size_t hash_table_client::num_keys() {
  for (auto &block : blocks_) {
    block.second->send_command(hash_table_cmd_id::ht_num_keys, {});
  }
  size_t n = 0;
  for (auto &block : blocks_) {
    n += std::stoll(block.second->recv_response().front());
  }
  return n;
}

std::size_t hash_table_client::block_id(const std::string &key) {
  return static_cast<size_t>((*std::prev(blocks_.upper_bound(hash_slot::get(key)))).first);
}

std::vector<std::string> hash_table_client::batch_command(const hash_table_cmd_id &op,
                                                          const std::vector<std::string> &args,
                                                          size_t args_per_op) {
  // Split arguments
  if (args.size() % args_per_op != 0)
    throw std::invalid_argument("Incorrect number of arguments");

  std::map<int32_t, std::vector<std::string>> block_args;
  std::map<int32_t, std::vector<size_t>> positions;
  size_t num_ops = args.size() / args_per_op;
  for (size_t i = 0; i < num_ops; i++) {
    auto id = block_id(args[i * args_per_op]);
    if (block_args.find(id) == block_args.end()) {
      block_args.emplace(std::make_pair(id, std::vector<std::string>{}));
    }
    if (positions.find(id) == positions.end()) {
      block_args.emplace(std::make_pair(id, std::vector<std::string>{}));
    }
    for (size_t j = 0; j < args_per_op; j++)
      block_args[id].push_back(args[i * args_per_op + j]);
    positions[id].push_back(i);
  }

  for (auto &block: blocks_) {
    if (!block_args[block.first].empty())
      block.second->send_command(op, block_args[block.first]);
  }

  std::vector<std::string> results(num_ops);
  for (auto &block: blocks_) {
    if (!block_args[block.first].empty()) {
      auto res = block.second->recv_response();
      for (size_t j = 0; j < res.size(); j++) {
        results[positions[block.first][j]] = res[j];
      }
    }
  }

  return results;
}

void hash_table_client::handle_redirect(int32_t cmd_id, const std::vector<std::string> &args, std::string &response) {
  if (response.substr(0, 10) == "!exporting") {
    //LOG(log_level::info) << "exporting redirect";
    typedef std::vector<std::string> list_t;
    do {
      auto parts = string_utils::split(response, '!');
      auto chain = list_t(parts.begin() + 2, parts.end());
      response = replica_chain_client(fs_,
                                      path_,
                                      directory::replica_chain(chain),
                                      KV_OPS,
                                      0).run_command_redirected(cmd_id, args).front();
    } while (response.substr(0, 10) == "!exporting");
  }
  if (response == "!block_moved") {
    //LOG(log_level::info) << "block_moved, refreshing";
    refresh();
    throw redo_error();
  }
  if (response == "!full") {
    //LOG(log_level::info) << "putting the client to sleep to let auto_scaling run first for 2^" << redo_times << " milliseconds";
    std::this_thread::sleep_for(std::chrono::milliseconds((int) (std::pow(2, redo_times))));
    redo_times++;
    throw redo_error();
  }
}

void hash_table_client::handle_redirects(int32_t cmd_id,
                                         const std::vector<std::string> &args,
                                         std::vector<std::string> &responses) {
  size_t n_ops = responses.size();
  size_t n_op_args = args.size() / n_ops;
  for (size_t i = 0; i < responses.size(); i++) {
    auto &response = responses[i];
    if (response.substr(0, 10) == "!exporting") {
      typedef std::vector<std::string> list_t;
      list_t op_args(args.begin() + i * n_op_args, args.begin() + (i + 1) * n_op_args);
      do {
        auto parts = string_utils::split(response, '!');
        auto chain = list_t(parts.begin() + 2, parts.end());
        response = replica_chain_client(fs_,
                                        path_,
                                        directory::replica_chain(chain),
                                        KV_OPS,
                                        0).run_command_redirected(cmd_id, op_args).front();
      } while (response.substr(0, 10) == "!exporting");
    }
    if (response == "!block_moved") {
      refresh();
      throw redo_error();
    }
    if (response == "!full") {
      std::this_thread::sleep_for(std::chrono::milliseconds((int) (std::pow(2, redo_times))));
      throw redo_error();
    }
  }
}

}
}