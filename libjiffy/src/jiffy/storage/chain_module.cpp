#include "chain_module.h"
#include "jiffy/utils/logger.h"
#include "jiffy/utils/time_utils.h"

namespace jiffy {
namespace storage {

using namespace utils;

chain_module::chain_module(block_memory_manager *manager,
                           const std::string &name,
                           const std::string &metadata,
                           const command_map &supported_cmds)
    : partition(manager, name, metadata, supported_cmds),
      next_(std::make_unique<next_chain_module_cxn>("nil")),
      prev_(std::make_unique<prev_chain_module_cxn>()),
      pending_(0) {}

chain_module::~chain_module() {
  next_->reset("nil");
  if (response_processor_.joinable())
    response_processor_.join();
}

void chain_module::setup(const std::string &path,
                         const std::vector<std::string> &chain,
                         chain_role role,
                         const std::string &next_block_id) {
  path_ = path;
  chain_ = chain;
  role_ = role;
  auto protocol = next_->reset(next_block_id);
  if (protocol && role_ != chain_role::tail) {
    auto handler = std::make_shared<chain_response_handler>(this);
    auto processor = std::make_shared<block_response_serviceProcessor>(handler);
    if (response_processor_.joinable())
      response_processor_.join();
    response_processor_ = std::thread([processor, protocol] {
      while (true) {
        try {
          if (!processor->process(protocol, protocol, nullptr)) {
            break;
          }
        } catch (std::exception &e) {
          break;
        }
      }
    });
  }
}

void chain_module::resend_pending() {
  auto ops = pending_.lock_table();
  try {
    for (const auto &op: ops) {
      next_->request(op.second.seq, op.second.args);
    }
  } catch (...) {
    ops.unlock();
    std::rethrow_exception(std::current_exception());
  }
  ops.unlock();
}

void chain_module::ack(const sequence_id &seq) {
  remove_pending(seq);
  if (!is_head()) {
    if (prev_ == nullptr) {
      LOG(log_level::error) << "Invalid state: Previous is null";
    }
    prev_->ack(seq);
  }
}

void chain_module::request(sequence_id seq, const arg_list &args) {
  if (!is_head() && !is_tail()) {
    LOG(log_level::error) << "Invalid state: Direct request on a mid node";
    return;
  }

  std::vector<std::string> result;
  if(args[0] == "dequeue") {
    LOG(log_level::info) << "Running dequeue " << is_tail();
  }
  run_command(result, args);
  if(args[0] == "dequeue") {
    LOG(log_level::info) << "Finish Running dequeue " << is_tail();
  }
  auto cmd_name = args.front();
  if (is_tail()) {
    const std::size_t max_data_size = 100 * 1024;
    std::vector<std::string> result_partial;
    if(args[0] == "dequeue" && (result.size() >= 2 && result[1].size() > max_data_size)) {
      LOG(log_level::info) << "This dequeue exceeds the size";
      auto data_to_send = result[1];
      std::size_t num_response = data_to_send.size() / max_data_size + (data_to_send.size() % max_data_size != 0);
      clients().respond_client(seq, std::vector<std::string>{std::to_string(num_response)});
      if(result.size() == 3)
        clients().respond_client(seq, std::vector<std::string>{result[0], result[2]});
      else
        clients().respond_client(seq, std::vector<std::string>{result[0]});
      for(std::size_t i = 0; i < num_response; i++) {
        LOG(log_level::info) << "Here 1";
        auto data_size = i * max_data_size + max_data_size > result[1].size() ? result[1].size() % max_data_size : max_data_size;
        LOG(log_level::info) << "Here 2";
        auto data_partial = result[1].substr(i * max_data_size, data_size);
        LOG(log_level::info) << "Here 3";
        result_partial.push_back(data_partial);
        LOG(log_level::info) << "Here 4";
        LOG(log_level::info) << "Sending partial data " << data_partial.size();
        LOG(log_level::info) << "Here 5";
        clients().respond_client(seq, result_partial);
        LOG(log_level::info) << "Here 6";
        result_partial.pop_back();
        LOG(log_level::info) << "Here 7";
      }
    } else {
      clients().respond_client(seq, result);
    }
    subscriptions().notify(cmd_name, args[1]); // TODO: Fix
  } else {
    if (is_accessor(cmd_name)) {
      LOG(log_level::error) << "Invalid state: Accessor request on non-tail node";
      return;
    }
    seq.server_seq_no = ++chain_seq_no_;
    next_->request(seq, args);
  }
  add_pending(seq, args);
}

void chain_module::chain_request(const sequence_id &seq, const arg_list &args) {
  auto cmd_name = args.front();
  if (is_head()) {
    LOG(log_level::error) << "Invalid state: Chain request " << cmd_name << " on head node";
    return;
  }
  if (is_accessor(cmd_name)) {
    LOG(log_level::error) << "Invalid state: Accessor " << cmd_name << " as chain request";
    return;
  }

  std::vector<std::string> result;
  run_command(result, args);

  if (is_tail()) {
    const std::size_t max_data_size = 512 * 1024;
    std::vector<std::string> result_partial;
    if(args[0] == "dequeue" && (result.size() >= 2 && result[1].size() > max_data_size)) {
      LOG(log_level::info) << "This dequeue exceeds the size";
      auto data_to_send = result[1];
      std::size_t num_response = data_to_send.size() / max_data_size + (data_to_send.size() % max_data_size != 0);
      clients().respond_client(seq, std::vector<std::string>{std::to_string(num_response)});
      if(result.size() == 3)
        clients().respond_client(seq, std::vector<std::string>{result[0], result[2]});
      else
        clients().respond_client(seq, std::vector<std::string>{result[0]});
      for(std::size_t i = 0; i < num_response; i++) {
        auto data_size = i * max_data_size + max_data_size > result[1].size() ? result[1].size() % max_data_size : max_data_size;
        auto data_partial = result[1].substr(i * max_data_size, data_size);
        result_partial.push_back(data_partial);
        LOG(log_level::info) << "Sending partial data " << data_partial.size();
        clients().respond_client(seq, result_partial);
        result_partial.pop_back();
      }
    } else {
      clients().respond_client(seq, result);
    }
    subscriptions().notify(cmd_name, args[1]); // TODO: Fix
    ack(seq);
  } else {
    // Do not need a lock since this is the only thread handling chain requests
    next_->request(seq, args);
  }
}

}
}
