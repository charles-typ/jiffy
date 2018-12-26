#ifndef MMUX_NOTIFICATION_SERVICE_HANDLER_H
#define MMUX_NOTIFICATION_SERVICE_HANDLER_H

#include "notification_service.h"
#include "subscription_service.h"
#include "subscription_map.h"
#include "../chain_module.h"
#include <thrift/transport/TSocket.h>

namespace mmux {
namespace storage {
/* Notification handler */
class notification_service_handler : public notification_serviceIf {
 public:

  /**
   * @brief Constructor
   * @param oprot Protocol
   * @param blocks Data blocks
   */

  explicit notification_service_handler(std::shared_ptr<::apache::thrift::protocol::TProtocol> oprot,
                                        std::vector<std::shared_ptr<chain_module>> &blocks);
  /**
   * @brief Subscribe to a block for given operations
   * This function adds all pairs of block and operations in local subscription set
   * and adds all the operation with the subscription service client to the subscription map
   * of the block
   *
   * @param block_id Block id number
   * @param ops Operations
   */

  void subscribe(int32_t block_id, const std::vector<std::string> &ops) override;

  /**
   * @brief Unsubscribe to the block for given operations
   * This function takes down all the operations that are unsubscribed
   * and clears local subscription, then it removes the subscription in
   * the block's subscription map
   *
   * @param block_id Block id, if block id is -1, find block id in local subscription
   * @param ops Operations, if ops is empty, clear all in local subscription, otherwise
   * just clear the specific operations
   */

  void unsubscribe(int32_t block_id, const std::vector<std::string> &ops) override;

 private:
  /* Local subscription set for pairs of block and operation */
  std::set<std::pair<int32_t, std::string>> local_subs_;
  /* Protocol */
  std::shared_ptr<::apache::thrift::protocol::TProtocol> oprot_;
  /* Subscription service client */
  std::shared_ptr<subscription_serviceClient> client_;
  /* Data blocks */
  std::vector<std::shared_ptr<chain_module>> &blocks_;
};

}
}

#endif //MMUX_NOTIFICATION_SERVICE_HANDLER_H