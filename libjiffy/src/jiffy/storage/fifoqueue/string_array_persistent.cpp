#include "string_array_persistent.h"
#include "jiffy/utils/logger.h"
#include "jiffy/utils/time_utils.h"
#include <chrono>
#include <thread>

namespace jiffy {
namespace storage {
using namespace utils;

string_array_persistent::string_array_persistent(std::string &path) : path_(path) {
  local_ = std::fstream(path, std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
  if(!local_.is_open()) {
      LOG(log_level::info) << "Error opening file";
  }
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
  return path_ == other.path_ && tail_ == other.tail_ && head_ == other.head_
      && last_element_offset_ == other.last_element_offset_;
}

void string_array_persistent::put(const std::string &item) {
  std::size_t len = item.size();
  // Write length
  local_.seekp(tail_, std::ios::beg);
  local_.write(reinterpret_cast<char*>(&len), METADATA_LEN);
  last_element_offset_ = tail_;
  tail_ += METADATA_LEN;
  // Write data
  local_.write(item.c_str(), len);
//  local_.flush();
  std::flush(local_);
  tail_ += len;
  //LOG(log_level::info) << "Writing to this position " << local_.tellp();
}

std::pair<bool, std::string> string_array_persistent::get() {
  if (empty()) {
    LOG(log_level::info) << "This persistent partition is empty";
    return std::make_pair(false, "");
  }
  std::size_t len;
  local_.seekg(head_, std::ios::beg);
  //LOG(log_level::info) << "Reading from this position " << local_.tellg();
  local_.read(reinterpret_cast<char*>(&len), METADATA_LEN);
  //LOG(log_level::info) << "The length to be read " << len;
  //std::string ret(len, '\0');
  char test[len];
  //local_.read(&ret[0], len);
  local_.read(&test[0], len);
  head_ += len + METADATA_LEN;
  std::string ret(test);
  return std::make_pair(true, std::string(test, len));
}

std::size_t string_array_persistent::find_next(std::size_t offset) {
  if (offset >= last_element_offset_ || offset >= tail_) return 0;
  std::size_t len;
  local_.read(reinterpret_cast<char*>(&len), METADATA_LEN);
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
  return tail_ == 0 || tail_ == (std::size_t(head_));
}

std::size_t string_array_persistent::max_offset() const {
  // TODO Not implemented
  return 0;
}


}
}

