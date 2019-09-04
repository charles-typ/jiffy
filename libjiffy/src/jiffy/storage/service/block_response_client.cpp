#include "block_response_client.h"
#include "jiffy/utils/logger.h"
#include "jiffy/utils/time_utils.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <iostream>

using namespace ::apache::thrift::protocol;

namespace jiffy {
namespace storage {
using namespace utils;
block_response_client::block_response_client(std::shared_ptr<TProtocol> protocol)
    : client_(std::make_shared<thrift_client>(protocol)) {}

void block_response_client::response(const sequence_id &seq, const std::vector<std::string> &result) {
	auto start = time_utils::now_us(); 
  client_->response(seq, result);
	auto end = time_utils::now_us(); 
	LOG(log_level::info) << "Send response took time " << end - start;
}

}
}
