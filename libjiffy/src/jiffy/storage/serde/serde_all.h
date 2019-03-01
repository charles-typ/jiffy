#ifndef JIFFY_SERDE_H
#define JIFFY_SERDE_H

#include "jiffy/storage/hashtable/hash_table_defs.h"
#include "jiffy/storage/btree/btree_defs.h"

namespace jiffy {
namespace storage {
/* Virtual class for Custom serializer/deserializer */
class serde {
 public:
//  typedef locked_hash_table_type block_type;
  virtual ~serde() = default;

  template <typename Datatype>
  std::size_t serialize(const Datatype &table, std::shared_ptr<std::ostream> out)
  {
    return Vser(table, out);
  }
  template  <typename Datatype>
  std::size_t deserialize(std::shared_ptr<std::istream> in, Datatype &table)
  {
    return Vdeser(in, table);
  }

 private:
  virtual std::size_t Vser(const locked_hash_table_type &table, std::shared_ptr<std::ostream> out) = 0;
  virtual std::size_t Vser(const btree_type &table, std::shared_ptr<std::ostream> out) = 0;
  virtual std::size_t Vdeser(std::shared_ptr<std::istream> in, locked_hash_table_type &table) = 0;
  virtual std::size_t Vdeser(std::shared_ptr<std::istream> in, btree_type &table) = 0;
};

template <class Impl>
class Derived : public Impl {
 public:
  template <class... TArgs>
      Derived(TArgs&&... args): Impl(std::forward<TArgs>(args)...)
  {

  }
 private:
  std::size_t Vser(const locked_hash_table_type &table, std::shared_ptr<std::ostream> out) {
    return Impl::Tser(table, out);
  }
  std::size_t Vser(const btree_type &table, std::shared_ptr<std::ostream> out) {
    return Impl::Tser(table, out);
  }
  std::size_t Vdeser(std::shared_ptr<std::istream> in, locked_hash_table_type &table) {
    return Impl::Tdeser(in, table);
  }
  std::size_t Vdeser(std::shared_ptr<std::istream> in, btree_type &table) {
    return Impl::Tdeser(in, table);
  }
};

/* CSV serializer/deserializer class
 * Inherited from serde class */

class csv_serde_Impl : public serde {
 public:
  csv_serde() = default;

  virtual ~csv_serde() = default;
 protected:

  /**
   * @brief Serialize hash table in CSV format
   * @param table Locked hash table
   * @param path Output stream
   * @return Output stream position after flushing
   */
  template <typename Datatype>
  std::size_t Tser(const Datatype &table, std::shared_ptr<std::ostream> path) override;

  /**
   * @brief Deserialize Input stream to hash table in CSV format
   * @param in Input stream
   * @param table Locked hash table
   * @return Input stream position after reading
   */
  template <typename Datatype>
  std::size_t Tdeser(std::shared_ptr<std::istream> in, Datatype &table) override;

 private:

  /**
   * @brief Split the string separated by separation symbol in count parts
   * @param s String
   * @param delim Separation symbol
   * @param count Count
   * @return Split result
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
   * @return Split result
   */

  inline std::vector<std::string> split(const std::string &s, char delim) {
    return split(s, delim, UINT64_MAX);
  }
};

using csv_serde = Derived<csv_serde_Impl>;

/* Binary serializer/deserializer class
 * Inherited from serde class */
class binary_serde_Impl : public serde {
 public:
  binary_serde() = default;

  virtual ~binary_serde() = default;

 protected:
  /**
   * @brief Binary serialization
   * @param table Locked hash table
   * @param out Output stream
   * @return Output stream position
   */
  template <typename Datatype>
  size_t Tser(const Datatype &table, std::shared_ptr<std::ostream> out) override;

  /**
   * @brief Binary deserialization
   * @param in Input stream
   * @param table Locked hash table
   * @return Input stream position
   */
  template <typename Datatype>
  size_t Tdeser(std::shared_ptr<std::istream> in, Datatype &table) override;
};



using binary_serde = Derived<binary_serde_Impl>;

}
}

#endif //JIFFY_SERDE_H
