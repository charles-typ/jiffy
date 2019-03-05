#include "msg_queue_client.h"
#include "jiffy/utils/logger.h"
#include "jiffy/utils/string_utils.h"


namespace jiffy {
namespace storage {

using namespace jiffy::utils;

msg_queue_client::msg_queue_client(std::shared_ptr<directory::directory_interface> fs,
                           const std::string &path,
                           const directory::data_status &status,
                           int timeout_ms)
    : fs_(std::move(fs)), path_(path), status_(status), timeout_ms_(timeout_ms) {
  blocks_.clear();
  for (const auto &block: status.data_blocks()) {
    //slots_.push_back(std::stoull(utils::string_utils::split(block.name, '_')[0])); TODO currently we assume that all the blocks have the key range from MINKEY to MAXKEY
    blocks_.push_back(std::make_shared<replica_chain_client>(fs_, path_, block, timeout_ms_));
  }
}

directory::data_status &msg_queue_client::status() {
  return status_;
}


void msg_queue_client::refresh() {
  status_ = fs_->dstatus(path_);
  LOG(log_level::info) << "Refreshing partition mappings to " << status_.to_string();
  blocks_.clear();
  for (const auto &block: status_.data_blocks()) {
    blocks_.push_back(std::make_shared<replica_chain_client>(fs_, path_, block, timeout_ms_));
  }
}

std::string msg_queue_client::send(const std::string &msg) {
  std::string _return;
  std::vector<std::string> args{msg};
  bool redo;
  do {
    try {
      _return = blocks_[block_id(0)]->run_command(msg_queue_cmd_id::mq_send, args).front(); // TODO hot fix, should use block id
      // handle_redirect(b_tree_cmd_id::bt_put, args, _return);
      redo = false;
    } catch (redo_error &e) {
      redo = true;
    }
  } while (redo);
  return _return;
}

std::string msg_queue_client::receive() {
  std::string _return;
  std::vector<std::string> args;
  args.push_back(get_inc_receive_pos());
  bool redo;
  do {
    try {
      _return = blocks_[0]->run_command(msg_queue_cmd_id::mq_receive, args).front();// TODO hot fix, should use block id
      // handle_redirect(b_tree_cmd_id::bt_put, args, _return);
      redo = false;
    } catch (redo_error &e) {
      redo = true;
    }
  } while (redo);
  return _return;
}

std::vector<std::string>  msg_queue_client::send(const std::vector<std::string> &msgs) {
  if (msgs.size() == 0) {
    throw std::invalid_argument("Incorrect number of arguments");
  }
  std::vector<std::string> _return;
  bool redo;
  do {
    try {
      _return = batch_command(msg_queue_cmd_id::mq_send, msgs, 1);
      //  handle_redirects(b_tree_cmd_id::bt_put, kvs, _return);
      redo = false;
    } catch (redo_error &e) {
      redo = true;
    }
  } while (redo);
  return _return;
}

std::vector<std::string>  msg_queue_client::receive(std::size_t num_msg) {
  //if (kvs.size() % 2 != 0) {  TODO add check here with rend, the client cannot read beyond the latest message
  //  throw std::invalid_argument("Incorrect number of arguments");
  //}
  std::vector<std::string> args;
  std::vector<std::string> _return;
  for(std::size_t i = 0; i < num_msg; i++) {
    args.push_back(get_inc_receive_pos());
  }
  bool redo;
  do {
    try {
      _return = batch_command(msg_queue_cmd_id::mq_receive, args, 1);
      //  handle_redirects(b_tree_cmd_id::bt_put, kvs, _return);
      redo = false;
    } catch (redo_error &e) {
      redo = true;
    }
  } while (redo);
  return _return;
}

//TODO fix this function
size_t msg_queue_client::block_id(const std::string &key) {
  return 0;
  //return static_cast<size_t>(std::upper_bound(slots_.begin(), slots_.end(), hash_slot::get(key)) - slots_.begin() - 1);
}

std::vector<std::string> msg_queue_client::batch_command(const msg_queue_cmd_id &op,
                                                     const std::vector<std::string> &args,
                                                     size_t args_per_op) {
  // Split arguments
  if (args.size() % args_per_op != 0)
    throw std::invalid_argument("Incorrect number of arguments");

  std::vector<std::vector<std::string>> block_args(blocks_.size());
  std::vector<std::vector<size_t>> positions(blocks_.size());
  size_t num_ops = args.size() / args_per_op;
  for (size_t i = 0; i < num_ops; i++) {
    auto id = block_id(args[i * args_per_op]);
    for (size_t j = 0; j < args_per_op; j++)
      block_args[id].push_back(args[i * args_per_op + j]);
    positions[id].push_back(i);
  }

  for (size_t i = 0; i < blocks_.size(); i++) {
    if (!block_args[i].empty())
      blocks_[i]->send_command(op, block_args[i]);
  }

  std::vector<std::string> results(num_ops);
  for (size_t i = 0; i < blocks_.size(); i++) {
    if (!block_args[i].empty()) {
      auto res = blocks_[i]->recv_response();
      for (size_t j = 0; j < res.size(); j++) {
        results[positions[i][j]] = res[j];
      }
    }
  }

  return results;
}
/* Remove for now
void msg_queue_client::handle_redirect(int32_t cmd_id, const std::vector<std::string> &args, std::string &response) {
  if (response.substr(0, 10) == "!exporting") {
    typedef std::vector<std::string> list_t;
    do {
      auto parts = string_utils::split(response, '!');
      auto chain = list_t(parts.begin() + 2, parts.end());
      response = replica_chain_client(fs_,
                                      path_,
                                      directory::replica_chain(chain),
                                      0).run_command_redirected(cmd_id, args).front();
    } while (response.substr(0, 10) == "!exporting");
  }
  if (response == "!block_moved") {
    refresh();
    throw redo_error();
  }
}

void msg_queue_client::handle_redirects(int32_t cmd_id,
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
                                        0).run_command_redirected(cmd_id, op_args).front();
      } while (response.substr(0, 10) == "!exporting");
    }
    if (response == "!block_moved") {
      refresh();
      throw redo_error();
    }
  }
}
*/

}
}