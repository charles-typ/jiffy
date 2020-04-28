#ifndef JIFFY_STRING_ARRAY_PERSISTENT_H
#define JIFFY_STRING_ARRAY_PERSISTENT_H

#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <iterator>
#include "jiffy/storage/block_memory_allocator.h"

namespace jiffy {
namespace storage {

/**
 * @brief String array class
 *
 * This data structure store strings in "length | string" format
 * and supports storing big strings between different data blocks.
 */
class string_array_persistent {
  typedef std::string value_type;
  typedef std::string *pointer;

 public:
  static const int METADATA_LEN = sizeof(std::size_t);

  /**
   * @brief Constructor
   */
  string_array_persistent() = default;

  /**
   * @brief Constructor
   * @param max_size Max size for the string array
   * @param alloc Block memory allocator
   */
  explicit string_array_persistent(std::string &path);

  /**
   * @brief Destructor
   */
  ~string_array_persistent();

  /**
   * @brief Equal operator
   * @param other Another string array
   * @return Boolean, true if equal
   */
  bool operator==(const string_array_persistent &other) const;

  /**
   * @brief Push new message at the end of the array
   * @param item Message
   * @return Pair, a status boolean and the remain string
   */
  void put(const std::string &item);

  /**
   * @brief Read string at offset
   * @param offset Read offset
   * @param Pair, a status boolean and the read string
   */
  std::pair<bool, std::string> get();

  /**
   * @brief Find next string for the given offset string
   * @param offset Offset of the current string
   * @return Offset of the next string
   */
  std::size_t find_next(std::size_t offset);

  /**
   * @brief Fetch total size of the string array
   * @return Size
   */
  std::size_t size() const;

  /**
   * @brief Fetch last element offset in the partition
   * @return Last element offset
   */
  std::size_t last_element_offset() const;

  /**
   * @brief Fetch capacity of the string array
   * @return Capacity
   */
  std::size_t capacity();

  /**
   * @brief Clear the content of string array
   */
  void clear();

  /**
   * @brief Check if string array is empty
   * @return Boolean, true if empty
   */
  bool empty() const;

  /**
   * @brief Fetch the maximum offset of the strings
   * @return Maximum offset of the strings
   */
  std::size_t max_offset() const;

  /**
   * @brief Fetch the number of elements in the string array
   * @return Number of elements
   */
  std::size_t num_elements() const;

 private:

  /* Read pointer */
  std::streamoff head_;

  /* File path */
  std::string path_;

  /* Local store */
  std::fstream local_;

  /* Offset of the last element */
  std::size_t last_element_offset_;

  /* Tail position */
  std::size_t tail_{};

};


}
}


#endif //JIFFY_STRING_ARRAY_PERSISTENT_H
