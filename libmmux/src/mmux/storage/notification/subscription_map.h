#ifndef MMUX_SUBSCRIPTION_MAP_H
#define MMUX_SUBSCRIPTION_MAP_H

#include <string>
#include <unordered_map>
#include <mutex>
#include "subscription_service.h"
namespace mmux {
namespace storage {
/* Subscription map class
 * This map records all the clients that are waiting for a specific operation
 *  on the block. When the operation is done, the block will send a notification
 *  in order to let the client get the right data at right time
 * */
class subscription_map {
 public:
  /**
   * @brief Constructor
   */

  subscription_map();

  /**
   * @brief Add operations to subscription map
   * The subscription service client will generate a response
   * @param ops Operations
   * @param client Subscription service client
   */

  void add_subscriptions(const std::vector<std::string> &ops, std::shared_ptr<subscription_serviceClient> client);

  /**
   * @brief Remove a subscription from subscription map
   * If inform is true, subscription service client will generate a response
   * @param ops Operations
   * @param client Subscription service client
   * @param inform Bool value that indicates inform or not
   */

  void remove_subscriptions(const std::vector<std::string> &ops,
                            std::shared_ptr<subscription_serviceClient> client,
                            bool inform = true);

  /**
   * @brief Notify all the waiting clients of the operation
   * @param op Operation
   * @param msg Message to be sent to waiting clients
   */

  void notify(const std::string &op, const std::string &msg);

  /**
   * @brief Clear the subscription map
   */

  void clear();

 private:
  /* Subscription mapp operation mutex */
  std::mutex mtx_{};
  /* Subscription map */
  std::map<std::string, std::set<std::shared_ptr<subscription_serviceClient>>> subs_{};
};

}
}

#endif //MMUX_SUBSCRIPTION_MAP_H
