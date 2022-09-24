/*
 * ============================================================================
 *
 *        Authors:  Prashant Pandey <ppandey@cs.stonybrook.edu>
 *                  Rob Johnson <robj@vmware.com>   
 *                  Bradley C. Kuszmaul <kuszmaul@gmail.com>
 *
 * ============================================================================
 */
#ifndef EXPERIMENTAL_USERS_BRADLEYBEAR_CQF_COUNTING_QUOTIENT_FILTER_H_
#define EXPERIMENTAL_USERS_BRADLEYBEAR_CQF_COUNTING_QUOTIENT_FILTER_H_

#include <cstddef>
#include <cstdint>
#include <optional>

#include "experimental/users/bradleybear/cqf/include/gqf.h"
#include "experimental/users/bradleybear/cqf/include/gqf_int.h"

namespace compact_quotient_filter {

class CompactQuotientFilter {
 public:
  CompactQuotientFilter() = default;
  explicit CompactQuotientFilter(size_t nslots, uint64_t key_bits, uint64_t value_bits);
  void reserve(size_t nslots);
  void insert(uint64_t key, uint64_t value, uint64_t count);
  void set_count(uint64_t key, uint64_t value, uint64_t count);
  void decrement_or_erase(uint64_t key, uint64_t value, uint64_t count);
  void erase(uint64_t key, uint64_t value);
  // Returns the first value associated with key (if there isn't one, then
  // returns nullopt.)
  std::optional<uint64_t> query(uint64_t key) const;
  size_t count(uint64_t key, uint64_t value) const;
  std::optional<size_t> get_unique_index(uint64_t key, uint64_t value) const;
  class iterator;
  using const_iterator = iterator;
  iterator begin() const;
  const_iterator cbegin() const;
  iterator end() const;
  const_iterator cend() const;
};

class CompactQuotientFilter::iterator {
 private:
  friend CompactQuotientFilter;
  // The C version of the iterator doens't have an "end" it has a check to see
  // if it is at the end.  And no way to build the end iterator.
  bool is_end_; 
  quotient_filter_iterator c_iterator_;
};

}  // namespace compact_quotient_filter  

#endif  // EXPERIMENTAL_USERS_BRADLEYBEAR_CQF_COUNTING_QUOTIENT_FILTER_H_
