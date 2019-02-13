#ifndef JIFFY_BUFFERED_TRANSPORT_H
#define JIFFY_BUFFERED_TRANSPORT_H

#include <thrift/transport/TTransport.h>
#include <thrift/transport/TBufferTransports.h>

namespace jiffy {
namespace storage {

/* Buffered transport factory class */
class BufferedTransportFactory : public apache::thrift::transport::TTransportFactory {
 public:
  /**
   * @brief Constructor
   * @param buffer_size Buffer size
   */

  explicit BufferedTransportFactory(uint32_t buffer_size) : buffer_size_(buffer_size) {}

  /**
   * @brief Virtual destructor
   */

  virtual ~BufferedTransportFactory() {}

  /**
   * Wraps the transport into a buffered one.
   */

  std::shared_ptr<apache::thrift::transport::TTransport> getTransport(std::shared_ptr<apache::thrift::transport::TTransport> trans) override {
    return std::shared_ptr<apache::thrift::transport::TTransport>(new apache::thrift::transport::TBufferedTransport(
        trans,
        buffer_size_));
  }

 private:
  /* Buffer size */
  uint32_t buffer_size_;
};
}
}

#endif //JIFFY_BUFFERED_TRANSPORT_H
