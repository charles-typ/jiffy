#include "string_array_persistent.h"
#include "jiffy/utils/logger.h"

namespace jiffy {
namespace storage {
using namespace utils;

string_array_persistent::string_array_persistent(std::string &path) : path_(path) {
  local_ = ifstream(path, ios::in | ios::out | ios::trunc); 
  tail_ = 0;
  last_element_offset_ = 0;
}

string_array_persistent::~string_array_persistent() {
  local_.close();
}

string_array_persistent::string_array_persistent(const string_array_persistent &other) {
  local_ = other.local_;
  path_ = other.path_;
  tail_ = other.tail_;
  split_string_ = other.split_string_;
  last_element_offset_ = other.last_element_offset_;
}

string_array_persistent &string_array_persistent::operator=(const string_array_persistent &other) {
  local_ = other.local_;
  tail_ = other.tail_;
  path_ = other.path_;
  last_element_offset_ = other.last_element_offset_;
  split_string_ = other.split_string_;
  return *this;
}

bool string_array_persistent::operator==(const string_array_persistent &other) const {
  return path_ == other.path_ && tail_ == other.tail_ 
      && last_element_offset_ == other.last_element_offset_
      && split_string_ == other.split_string_;
}

std::pair<bool, std::string> string_array_persistent::push_back(const std::string &item) {
  auto len = item.size();
    // Write length
  std::memcpy(data_ + tail_, (char *) &len, METADATA_LEN);
  last_element_offset_ = tail_;
  tail_ += METADATA_LEN;

  // Write data
  std::memcpy(data_ + tail_, item.c_str(), len);
  tail_ += len;
  return std::make_pair(true, std::string("!success"));
}

const std::pair<bool, std::string> string_array_persistent::at(std::size_t offset) const {
  if (offset > last_element_offset_ || empty()) {
    if (split_string_)
      return std::make_pair(false, "");
    return std::make_pair(false, std::string("!not_available"));
  }
  auto len = *((std::size_t *) (data_ + offset));
  return std::make_pair(true, std::string(data_ + offset + METADATA_LEN, len));
}

std::size_t string_array_persistent::find_next(std::size_t offset) const {
  if (offset >= last_element_offset_ || offset >= tail_) return 0;
  return offset + *reinterpret_cast<size_t*>(data_ + offset) + METADATA_LEN;
}

std::size_t string_array_persistent::size() const {
  return tail_;
}

std::size_t string_array_persistent::last_element_offset() const {
  return last_element_offset_;
}

std::size_t string_array_persistent::capacity() {
  return max_ - METADATA_LEN;
}

void string_array_persistent::clear() {
  tail_ = 0;
  last_element_offset_ = 0;
}

bool string_array_persistent::empty() const {
  return tail_ == 0;
}

string_array_persistent::iterator string_array_persistent::begin() {
  return string_array_persistent::iterator(*this, 0);
}

string_array_persistent::iterator string_array_persistent::end() {
  return string_array_persistent::iterator(*this, max_);
}

std::size_t string_array_persistent::max_offset() const {
  return max_;
}

string_array_persistent::const_iterator string_array_persistent::begin() const {
  return string_array_persistent::const_iterator(*this, 0);
}

string_array_persistent::const_iterator string_array_persistent::end() const {
  return string_array_persistent::const_iterator(*this, max_);
}
bool string_array_persistent::full() const {
  return split_string_;
}

string_array_persistent_iterator::string_array_persistent_iterator(string_array_persistent &impl, std::size_t pos)
    : impl_(impl),
      pos_(pos) {}

string_array_persistent_iterator::value_type string_array_persistent_iterator::operator*() const {
  auto ret = impl_.at(pos_);
  return ret.second;
}

const string_array_persistent_iterator string_array_persistent_iterator::operator++(int) {
  pos_ = impl_.find_next(pos_);
  if (pos_) {
    return *this;
  } else {
    pos_ = impl_.max_offset();
    return *this;
  }
}

bool string_array_persistent_iterator::operator==(string_array_persistent_iterator other) const {
  return (impl_ == other.impl_) && (pos_ == other.pos_);
}

bool string_array_persistent_iterator::operator!=(string_array_persistent_iterator other) const {
  return !(*this == other);
}

string_array_persistent_iterator &string_array_persistent_iterator::operator=(const string_array_persistent_iterator &other) {
  impl_ = other.impl_;
  pos_ = other.pos_;
  return *this;
}

const_string_array_persistent_iterator::const_string_array_persistent_iterator(const string_array_persistent &impl, std::size_t pos)
    : impl_(impl), pos_(pos) {}

const_string_array_persistent_iterator::value_type const_string_array_persistent_iterator::operator*() const {
  auto ret = impl_.at(pos_);
  return ret.second;
}

const const_string_array_persistent_iterator const_string_array_persistent_iterator::operator++(int) {
  pos_ = impl_.find_next(pos_);
  if (pos_) {
    return *this;
  } else {
    pos_ = impl_.max_offset();
    return *this;
  }
}

bool const_string_array_persistent_iterator::operator==(const_string_array_persistent_iterator other) const {
  return (impl_ == other.impl_) && (pos_ == other.pos_);
}

bool const_string_array_persistent_iterator::operator!=(const_string_array_persistent_iterator other) const {
  return !(*this == other);
}

}
}

