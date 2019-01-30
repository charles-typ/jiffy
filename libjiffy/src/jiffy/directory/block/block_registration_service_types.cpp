/**
 * Autogenerated by Thrift Compiler (0.11.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#include "block_registration_service_types.h"

#include <algorithm>
#include <ostream>

#include <thrift/TToString.h>

namespace jiffy { namespace directory {


block_registration_service_exception::~block_registration_service_exception() throw() {
}


void block_registration_service_exception::__set_msg(const std::string& val) {
  this->msg = val;
}
std::ostream& operator<<(std::ostream& out, const block_registration_service_exception& obj)
{
  obj.printTo(out);
  return out;
}


void swap(block_registration_service_exception &a, block_registration_service_exception &b) {
  using ::std::swap;
  swap(a.msg, b.msg);
  swap(a.__isset, b.__isset);
}

block_registration_service_exception::block_registration_service_exception(const block_registration_service_exception& other0) : TException() {
  msg = other0.msg;
  __isset = other0.__isset;
}
block_registration_service_exception& block_registration_service_exception::operator=(const block_registration_service_exception& other1) {
  msg = other1.msg;
  __isset = other1.__isset;
  return *this;
}
void block_registration_service_exception::printTo(std::ostream& out) const {
  using ::apache::thrift::to_string;
  out << "block_registration_service_exception(";
  out << "msg=" << to_string(msg);
  out << ")";
}

const char* block_registration_service_exception::what() const throw() {
  try {
    std::stringstream ss;
    ss << "TException - service has thrown: " << *this;
    this->thriftTExceptionMessageHolder_ = ss.str();
    return this->thriftTExceptionMessageHolder_.c_str();
  } catch (const std::exception&) {
    return "TException - service has thrown: block_registration_service_exception";
  }
}

}} // namespace
