#include <catch.hpp>
#include <thrift/transport/TTransportException.h>
#include <thread>
#include "test_utils.h"
#include "jiffy/directory/fs/directory_tree.h"
#include "jiffy/directory/fs/directory_server.h"
#include "jiffy/storage/manager/storage_management_server.h"
#include "jiffy/storage/manager/storage_management_client.h"
#include "jiffy/storage/manager/storage_manager.h"
#include "jiffy/storage/fifoqueue/fifo_queue_partition.h"
#include "jiffy/storage/service/block_server.h"
#include "jiffy/storage/client/fifo_queue_client.h"
#include "jiffy/auto_scaling/auto_scaling_client.h"
#include "jiffy/auto_scaling/auto_scaling_server.h"

using namespace ::jiffy::storage;
using namespace ::jiffy::directory;
using namespace ::jiffy::auto_scaling;
using namespace ::apache::thrift::transport;

#define NUM_BLOCKS 1
#define HOST "127.0.0.1"
#define DIRECTORY_SERVICE_PORT 9090
#define STORAGE_SERVICE_PORT 9091
#define STORAGE_MANAGEMENT_PORT 9092
#define AUTO_SCALING_SERVICE_PORT 9095

TEST_CASE("fifo_queue_multiple_test", "[enqueue][dequeue]") {
auto alloc = std::make_shared<sequential_block_allocator>();
auto block_names = test_utils::init_block_names(64, STORAGE_SERVICE_PORT, STORAGE_MANAGEMENT_PORT);
alloc->add_blocks(block_names);
auto blocks = test_utils::init_fifo_queue_blocks(block_names);

auto storage_server = block_server::create(blocks, STORAGE_SERVICE_PORT);
std::thread storage_serve_thread([&storage_server] { storage_server->serve(); });
test_utils::wait_till_server_ready(HOST, STORAGE_SERVICE_PORT);

auto mgmt_server = storage_management_server::create(blocks, HOST, STORAGE_MANAGEMENT_PORT);
std::thread mgmt_serve_thread([&mgmt_server] { mgmt_server->serve(); });
test_utils::wait_till_server_ready(HOST, STORAGE_MANAGEMENT_PORT);

auto as_server = auto_scaling_server::create(HOST, DIRECTORY_SERVICE_PORT, HOST, AUTO_SCALING_SERVICE_PORT);
std::thread auto_scaling_thread([&as_server]{as_server->serve(); });
test_utils::wait_till_server_ready(HOST, AUTO_SCALING_SERVICE_PORT);

auto sm = std::make_shared<storage_manager>();
auto tree = std::make_shared<directory_tree>(alloc, sm);

auto dir_server = directory_server::create(tree, HOST, DIRECTORY_SERVICE_PORT);
std::thread dir_serve_thread([&dir_server] { dir_server->serve(); });
test_utils::wait_till_server_ready(HOST, DIRECTORY_SERVICE_PORT);

data_status status = tree->create("/sandbox/file.txt", "fifoqueue", "/tmp", NUM_BLOCKS, 1, 0, 0,
                                  {"0"}, {"regular"});
uint32_t num_ops = 1000;
uint32_t data_size = 102400;
const uint32_t num_threads = 5;
std::vector<std::thread> workers;
std::vector<std::thread> de_workers;

for (uint32_t i = 1; i <= num_threads; i++) {
  workers.push_back(std::thread([i, &tree, &status, num_ops, data_size] {
  std::string data_(data_size, std::to_string(i)[0]);
  fifo_queue_client client(tree, "/sandbox/file.txt", status);
  for (uint32_t j = 0; j < num_ops; j++) {
    REQUIRE_NOTHROW(client.enqueue(data_));
    }
  }));
}
//for (std::thread &worker : workers) {
//  worker.join();
//}

std::vector<int> count = std::vector<int>(num_threads, 0);
for(uint32_t k = 1; k <= 1; k++) {
  workers.push_back(std::thread([k, &tree, &status, num_ops, &count] {
  fifo_queue_client client(tree, "/sandbox/file.txt", status);
  for (uint32_t j = 0; j < num_ops * num_threads * 10 ; j++) {
    std::string ret;
    REQUIRE_NOTHROW(ret = client.dequeue());
    if(ret != "!msg_not_found") count[k - 1]++;
  }
}));
}


for (std::thread &worker : workers) {
  worker.join();
}

int read_count = 0;

for(auto &x : count)
  read_count += x;

std::cout << read_count << std::endl;
as_server->stop();
if(auto_scaling_thread.joinable()) {
auto_scaling_thread.join();
}
storage_server->stop();
if (storage_serve_thread.joinable()) {
storage_serve_thread.join();
}

mgmt_server->stop();
if (mgmt_serve_thread.joinable()) {
mgmt_serve_thread.join();
}
dir_server->stop();
if (dir_serve_thread.joinable()) {
dir_serve_thread.join();


}
}
