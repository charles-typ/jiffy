#ifndef JIFFY_B_TREE_H
#define JIFFY_B_TREE_H

#include <functional>
//#include "libcuckoo/cuckoohash_map.hh" TODO needs to be replaced by the new btree source
#include "jiffy/storage/block_memory_allocator.h"

namespace jiffy {
namespace storage {

// The default number of elements in an empty btree
//constexpr size_t HASH_TABLE_DEFAULT_SIZE = 0;

// Btree definitions
//typedef std::string key_type;
//typedef std::string value_type;
//typedef std::pair<const key_type, value_type> kv_pair_type;

// Custom template arguments
//typedef std::hash<key_type> hash_type;
//typedef std::equal_to<std::string> equal_type;
//typedef block_memory_allocator<kv_pair_type> allocator_type;

// Hash table definitions TODO: needs to be replaced with B tree definitions
//typedef cuckoohash_map<key_type, value_type, hash_type, equal_type, allocator_type> hash_table_type;
//typedef hash_table_type::locked_table locked_hash_table_type;

}
}

#endif //JIFFY_B_TREE_H
