#ifndef JIFFY_MSG_QUEUE_CLIENT_H
#define JIFFY_MSG_QUEUE_CLIENT_H

#include "jiffy/directory/client/directory_client.h"
#include "jiffy/storage/client/replica_chain_client.h"
#include "jiffy/utils/client_cache.h"
#include "jiffy/storage/msgqueue/msg_queue_ops.h"

namespace jiffy {
namespace storage {

/* Redo when exception class
 * Redo whenever exception happens */
class redo_error : public std::exception {
 public:
  redo_error() = default;
};

class msg_queue_client {
 public:
  /**
   * @brief Constructor
   * Store all replica chain and their begin slot
   * @param fs Directory service
   * @param path Key value block path
   * @param status Data status
   * @param timeout_ms Timeout
   */

  msg_queue_client(std::shared_ptr<directory::directory_interface> fs,
               const std::string &path,
               const directory::data_status &status,
               int timeout_ms = 1000);


  /**
   * @brief Refresh the slot and blocks from directory service
   */

  void refresh();

  /**
   * @brief Fetch data status
   * @return Data status
   */

  directory::data_status &status();

  /**
   * @brief Send message
   * @param msg New message
   * @return Response of the command
   */

  std::string send(const std::string &msg);

  /**
   * @brief Receive message at the end position
   * @return Response of the command
   */

  std::string receive();

  /**
   * @brief Send message in batch
   * @param msgs New messages
   * @return Response of the commands
   */

  std::vector<std::string> send(const std::vector<std::string> &msgs);


  /**
   * @brief Receive message in batch
   * @param num_msg Number of message to be read in batch
   * @return Response of batch command
   */

  std::vector<std::string> receive(std::size_t num_msg);


 private:
  /**
   * @brief Get the receive start position and increase it by one
   * @return Start position in string
   */
  std::string get_inc_receive_pos() {
    auto old_val = rstart_;
    rstart_++;
    return std::to_string(old_val);
  }

  /**
   * @brief Fetch block identifier for particular key
   * @param key Key
   * @return Block identifier
   */

  size_t block_id(const std::string &key);

  /**
   * @brief Run same operation in batch
   * @param id Operation identifier
   * @param args Operation arguments
   * @param args_per_op Argument per operation
   * @return
   */

  std::vector<std::string> batch_command(const msg_queue_cmd_id &id, const std::vector<std::string> &args, size_t args_per_op);

  /**
   * @brief Handle command in redirect case
   * @param cmd_id Command identifier
   * @param args Command arguments
   * @param response Response to be collected
   */

//  void handle_redirect(int32_t cmd_id, const std::vector<std::string> &args, std::string &response);

  /**
   * @brief Handle multiple commands in redirect case
   * @param cmd_id Command identifier
   * @param args Command arguments
   * @param responses Responses to be collected
   */

//  void handle_redirects(int32_t cmd_id, const std::vector<std::string> &args, std::vector<std::string> &responses);

  /* Directory client */
  std::shared_ptr<directory::directory_interface> fs_;
  /* Key value partition path */
  std::string path_;
  /* Read start */
  std::size_t rstart_; // TODO add usage
  /* Read End */
  std::size_t rend_;   // TODO add usage
  /* Data status */
  directory::data_status status_;
  /* Replica chain clients, each partition only save a replica chain client */
  std::vector<std::shared_ptr<replica_chain_client>> blocks_;
  /* Time out*/
  int timeout_ms_;



};

}
}

#endif //JIFFY_MSG_QUEUE_CLIENT_H