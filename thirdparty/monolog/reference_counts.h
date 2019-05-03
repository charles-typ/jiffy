#ifndef CONFLUO_STORAGE_REFERENCE_COUNTS_H_
#define CONFLUO_STORAGE_REFERENCE_COUNTS_H_

#include <atomic.h>

namespace confluo {
namespace storage {

/**
 * Packs two 16-bit ref counts in a single 32-bit integer.
 */
class reference_counts {
 public:

  reference_counts()
      : ref_counts_(BOTH_DELTA) {
  }

  reference_counts(reference_counts &other)
      : ref_counts_(atomic::load(&other.ref_counts_)) {
  }

  reference_counts operator=(reference_counts &other) {
    atomic::store(&ref_counts_, atomic::load(&other.ref_counts_));
    return *this;
  }

  void increment_first() {
    atomic::faa(&ref_counts_, FIRST_DELTA);
  }

  void increment_second() {
    atomic::faa(&ref_counts_, SECOND_DELTA);
  }

  void increment_both() {
    atomic::faa(&ref_counts_, BOTH_DELTA);
  }

  void decrement_first() {
    atomic::fas(&ref_counts_, FIRST_DELTA);
  }

  void decrement_second() {
    atomic::fas(&ref_counts_, SECOND_DELTA);
  }

  void decrement_both() {
    atomic::fas(&ref_counts_, BOTH_DELTA);
  }

  bool decrement_first_and_compare() {
    return (atomic::fas(&ref_counts_, 1U) & FIRST_MASK) == 1;
  }

  bool decrement_second_and_compare() {
    return (atomic::fas(&ref_counts_, SECOND_DELTA) >> SECOND_SHIFT) == 1;
  }

  uint32_t get_first() {
    return atomic::load(&ref_counts_) & FIRST_MASK;
  }

  uint32_t get_second() {
    return atomic::load(&ref_counts_) >> SECOND_SHIFT;
  }

 private:
  atomic::type<uint32_t> ref_counts_;

  static const uint32_t FIRST_DELTA = 1U;
  static const uint32_t SECOND_DELTA = 1U << 16;
  static const uint32_t BOTH_DELTA = FIRST_DELTA + SECOND_DELTA;
  static const uint32_t FIRST_MASK = (1U << 16) - 1;
  static const uint32_t SECOND_SHIFT = 16;

};

}
}

#endif /* CONFLUO_STORAGE_REFERENCE_COUNTS_H_ */

