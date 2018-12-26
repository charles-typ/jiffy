#ifndef MMUX_PERSISTENT_STORE_H
#define MMUX_PERSISTENT_STORE_H

#include <memory>
#include "persistent_service.h"
#include "local/local_store.h"
#include "s3/s3_store.h"

namespace mmux {
namespace persistent {

class persistent_store {
 public:

  /**
   * @brief Fetch persistent service instance
   * @param path Path
   * @param ser Serialization options
   * @return Persistent service
   */

  static std::shared_ptr<persistent_service> instance(const std::string &path, std::shared_ptr<storage::serde> ser);

  /**
   * @brief Decompose path
   * @param path Path
   * @return (uri, key)
   */

  static std::pair<std::string, std::string> decompose_path(const std::string &path);

};

}
}

#endif //MMUX_PERSISTENT_STORE_H