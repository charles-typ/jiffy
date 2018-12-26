#ifndef MMUX_BROKER_SERVER_H
#define MMUX_BROKER_SERVER_H

#include "subscription_map.h"
#include "../chain_module.h"

#include <thrift/server/TThreadedServer.h>

namespace mmux {
namespace storage {
/* Notification server class */
class notification_server {
 public:

  /**
   * @brief Create notification server
   * @param blocks Chain modules
   * @param address Host address
   * @param port Port number
   * @return Server
   */

  static std::shared_ptr<apache::thrift::server::TThreadedServer> create(std::vector<std::shared_ptr<chain_module>> &blocks,
                                                                         const std::string &address,
                                                                         int port);
};

}
}

#endif //MMUX_BROKER_SERVER_H