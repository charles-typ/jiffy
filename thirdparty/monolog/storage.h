#ifndef CONFLUO_STORAGE_STORAGE_H_
#define CONFLUO_STORAGE_STORAGE_H_

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <errno.h>


#include "ptr_aux_block.h"
#include "storage_allocator.h"

#define PROT_RW PROT_READ | PROT_WRITE

namespace confluo {
namespace storage {
using namespace utils;
// TODO: Add documentation
// TODO: Improve allocation using pools and file consolidation.

/** Allocate function that allocates a file of a certain size */
typedef void *(*allocate_fn)(const std::string &path, size_t size);
/** Allocates a block in a file of a certain size */
typedef void *(*allocate_bucket_fn)(const std::string &path, size_t size);
/** Frees the given pointer */
typedef void (*free_fn)(void *ptr, size_t size);
/** Flushes the memory specified by the pointer */
typedef void (*flush_fn)(void *ptr, size_t size);

/**
 * Contains the particular storage mode
 */
enum storage_mode {
  IN_MEMORY = 0, /** Stores data in memory */
  DURABLE_RELAXED = 1, /** Has relaxed durability guarantees */
  DURABLE = 2 /** Persisted storage */
};

/**
 * Functionality for a storage function
 */
struct storage_functions {
  /** The particular storage mode */
  storage_mode mode;
  /** The allocation function */
  allocate_fn allocate;
  /** The function that allocates a bucket of memory */
  allocate_bucket_fn allocate_bucket;
  /** Function that frees memory */
  free_fn free;
  /** Function that flushes memory */
  flush_fn flush;
};

/**
 * In memory storage, uses malloc or mempool to allocate data that is not backed by a file.
 */
struct in_memory {
  /**
   * Allocates new memory.
   *
   * @param path Backing file (unused).
   * @param size Size of requested memory.
   * @return Allocated bytes.
   */
  inline static void *allocate(const std::string &path, size_t size) {
    return malloc(size);
  }

  /**
   * Allocates a bucket of new memory.
   *
   * @param path Backing file (unused).
   * @param size Size of requested block.
   * @return Allocated block.
   */
  inline static void *allocate_bucket(const std::string &path, size_t size) {
    ptr_aux_block aux(state_type::D_IN_MEMORY, encoding_type::D_UNENCODED);
    return allocator::instance().alloc(size, aux);
  }

  /**
   * Frees allocated memory.
   *
   * @param ptr Pointer to memory to be freed.
   * @param size Size of allocated memory.
   */
  inline static void free_mem(void *ptr, size_t size) {
    free(ptr);
  }

  /**
   * Flushes data to backed file (does nothing for this storage mode).
   *
   * @param ptr Pointer to memory.
   * @param size Size of allocated memory.
   */
  inline static void flush(void *ptr, size_t size) {
    return;
  }
};


class storage_mode_functions {
 public:
  /** Storage functionality for in memory mode */
  static storage_functions &IN_MEMORY_FNS();

  /** Contains the storage functions for all storage modes */
  static storage_functions *STORAGE_FNS();
};

}
}

#endif /* CONFLUO_STORAGE_STORAGE_H_ */
