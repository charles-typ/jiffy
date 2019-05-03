#include "catch.hpp"
#include "jiffy/storage/file/file_ops.h"
#include "jiffy/storage/file/file_defs.h"
#include "jiffy/storage/file/file_partition.h"
#include <vector>
#include <iostream>
#include <string>

using namespace ::jiffy::storage;
using namespace ::jiffy::persistent;

TEST_CASE("file_write_read_test", "[write][read]") {
  block_memory_manager manager;
  file_partition block(&manager);

  for (std::size_t i = 0; i < 1000; ++i) {
    REQUIRE(block.write(std::to_string(i)) == "!ok");
  }
  for (std::size_t i = 0; i < 1000; ++i) {
    REQUIRE(block.read(std::to_string(i)) == std::to_string(i));
  }
  for (std::size_t i = 1000; i < 2000; ++i) {
    REQUIRE(block.read(std::to_string(i)) == "!msg_not_found");
  }
}


TEST_CASE("file_write_clear_read_test", "[write][read]") {
  block_memory_manager manager;
  file_partition block(&manager);

  for (std::size_t i = 0; i < 1000; ++i) {
    REQUIRE(block.write(std::to_string(i)) == "!ok");
  }
  REQUIRE(block.clear() == "!ok");
  REQUIRE(block.size() == 0);
  for (std::size_t i = 0; i < 1000; ++i) {
    REQUIRE(block.read(std::to_string(i)) == "!msg_not_found");
  }
}

TEST_CASE("file_storage_size_test", "[put][size][storage_size][reset]") {
  block_memory_manager manager;
  file_partition block(&manager);
  for (std::size_t i = 0; i < 1000; ++i) {
    REQUIRE(block.write(std::to_string(i)) == "!ok");
  }
  REQUIRE(block.size() == 1000);
  //REQUIRE(block.storage_size() == 5780); Remove this test since we don't use bytes_ as storage size anymore
  REQUIRE(block.storage_size() <= block.storage_capacity());
}

TEST_CASE("file_flush_load_test", "[write][sync][reset][load][read]") {
  block_memory_manager manager;
  file_partition block(&manager);
  for (std::size_t i = 0; i < 1000; ++i) {
    std::vector<std::string> res;
    block.run_command(res, file_cmd_id::file_write, {std::to_string(i)});
    REQUIRE(res.front() == "!ok");
  }
  REQUIRE(block.is_dirty());
  REQUIRE(block.sync("local://tmp/test"));
  REQUIRE(!block.is_dirty());
  REQUIRE_FALSE(block.sync("local://tmp/test"));
  REQUIRE_NOTHROW(block.load("local://tmp/test"));
  for (std::size_t i = 0; i < 1000; ++i) {
    REQUIRE(block.read(std::to_string(i)) == std::to_string(i));
  }
}



