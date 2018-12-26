#ifndef MMUX_CSV_SERIALIZER_H
#define MMUX_CSV_SERIALIZER_H

#include <sstream>
#include "serde.h"

namespace mmux {
namespace storage {

/* CSV serialize and deserialize class
 * Inherited from serialize and deserialize class */

class csv_serde : public serde {
 public:
  csv_serde() = default;

  /**
   * @brief Serialize hash table in CSV format
   * @param table Locked hash table
   * @param path Ostream
   * @return Ostream position after flushing
   */

  size_t serialize(const block_type &table, std::shared_ptr<std::ostream> path) override;

  /**
   * @brief Deserialize Istream to hash table in CSV format
   * @param in Istream
   * @param table Locked hash table
   * @return Istream position after reading
   */

  size_t deserialize(std::shared_ptr<std::istream> in, block_type &table) override;

 private:

  /**
   * @brief Split the string separated by delim in count parts
   * @param s String
   * @param delim Separation symbol
   * @param count Part count
   * @return Vector of split result
   */

  inline std::vector<std::string> split(const std::string &s, char delim, size_t count) {
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> elems;
    size_t i = 0;
    while (std::getline(ss, item, delim) && i < count) {
      elems.push_back(std::move(item));
      i++;
    }
    while (std::getline(ss, item, delim))
      elems.back() += item;
    return elems;
  }

  /**
   * @brief Split with default count
   * @param s String
   * @param delim Separation symbol
   * @return Vector of split result
   */

  inline std::vector<std::string> split(const std::string &s, char delim) {
    return split(s, delim, UINT64_MAX);
  }
};

}
}

#endif //MMUX_CSV_SERIALIZER_H