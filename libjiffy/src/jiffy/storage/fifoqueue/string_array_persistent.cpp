#include "string_array_persistent.h"
#include "jiffy/utils/logger.h"

namespace jiffy {
namespace storage {
using namespace utils;

string_array_persistent::string_array_persistent(std::string &path) : path_(path) {
  local_ = std::fstream(path, std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
  local_.seekp(0, std::ios::beg);
  local_.seekg(0, std::ios::beg);
  tail_ = 0;
  head_ = 0;
  last_element_offset_ = 0;
}

string_array_persistent::~string_array_persistent() {
  local_.close();
}

bool string_array_persistent::operator==(const string_array_persistent &other) const {
  return path_ == other.path_ && tail_ == other.tail_ && head_ = other.head_
      && last_element_offset_ == other.last_element_offset_;
}

std::pair<bool, std::string> string_array_persistent::put(const std::string &item) {
  auto len = item.size();
  // Write length
  local_.seekp(0, std::ios::end);
  local_.write((char*)&len, METADATA_LEN);
  last_element_offset_ = tail_;
  tail_ += METADATA_LEN;

  // Write data
  local_.write(item.c_str(), len);
  tail_ += len;
  return std::make_pair(true, std::string("!success"));
}

const std::pair<bool, std::string> string_array_persistent::get() {
  if (empty()) {
    return std::make_pair(false, "");
  }
  char len_char[METADATA_LEN];
  local_.seekg(head_, std::ios::beg);
  local_.read(len_char, METADATA_LEN);
  auto len = *((std::size_t *) (len_char));
  char ret[len];
  local_.read(ret, len);
  head_ = head_ + len + METADATA_LEN;
  return std::make_pair(true, std::string(ret, len));
}

std::size_t string_array_persistent::find_next(std::size_t offset) const {
  if (offset >= last_element_offset_ || offset >= tail_) return 0;
  char len_char[METADATA_LEN];
  local_.read(len_char, METADATA_LEN);
  auto len = *((std::size_t *) (len_char));
  return offset + len + METADATA_LEN;
}

std::size_t string_array_persistent::size() const {
  return tail_;
}

std::size_t string_array_persistent::last_element_offset() const {
  return last_element_offset_;
}

std::size_t string_array_persistent::capacity() {
  // TODO not implemented
  return 0;
}

void string_array_persistent::clear() {
  tail_ = 0;
  last_element_offset_ = 0;
}

bool string_array_persistent::empty() const {
  return tail_ == 0 || tail_ == head_;
}

std::size_t string_array_persistent::max_offset() const {
  // TODO Not implemented
  return 0;
}


}
}

