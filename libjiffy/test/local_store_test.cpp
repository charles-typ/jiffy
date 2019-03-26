#include <fstream>
#include <catch.hpp>
#include <iostream>
#include "jiffy/persistent/persistent_service.h"
#include "jiffy/utils/directory_utils.h"
#include "jiffy/storage/serde/serde_all.h"

using namespace ::jiffy::persistent;
using namespace ::jiffy::storage;

binary make_binary(const std::string& str, block_memory_allocator<uint8_t> allocator) {
  return binary(str, allocator);
}

TEST_CASE("local_write_test", "[write]") {
  block_memory_manager manager;
  block_memory_allocator<kv_pair_type> allocator(&manager);
  block_memory_allocator<uint8_t> binary_allocator(&manager);
  //hash_table_type table(HASH_TABLE_DEFAULT_SIZE, hash_type(), equal_type(), allocator);
  hash_table_type table(HASH_TABLE_DEFAULT_SIZE, hash_type(), equal_type());
  auto ltable = table.lock_table();
  auto bkey = make_binary("key", binary_allocator);
  auto bval = make_binary("value", binary_allocator);
  ltable.insert(bkey, bval);
  auto ser = std::make_shared<csv_serde>(binary_allocator);
  local_store store(ser);
  REQUIRE_NOTHROW(store.write(ltable, "/tmp/a.txt"));
  ltable.unlock();
  std::ifstream in("/tmp/a.txt", std::ifstream::in);
  std::string data;
  in >> data;
  REQUIRE(data == "key,value");
  std::remove("/tmp/a.txt");
}

TEST_CASE("local_read_test", "[read]") {
  std::ofstream out("/tmp/a.txt", std::ofstream::out);
  out << "key,value\n";
  out.close();
  block_memory_manager manager;
  block_memory_allocator<kv_pair_type> allocator(&manager);
  block_memory_allocator<uint8_t> binary_allocator(&manager);
  auto ser = std::make_shared<csv_serde>(binary_allocator);
  local_store store(ser);
  //hash_table_type table(HASH_TABLE_DEFAULT_SIZE, hash_type(), equal_type(), allocator);
  hash_table_type table(HASH_TABLE_DEFAULT_SIZE, hash_type(), equal_type());
  auto ltable = table.lock_table();
  auto bkey = make_binary("key", binary_allocator);
  auto bval = make_binary("value", binary_allocator);
  REQUIRE_NOTHROW(store.read("/tmp/a.txt", ltable));
  REQUIRE(ltable.at(bkey) == bval);
  ltable.unlock();
  std::remove("/tmp/a.txt");
}
