
#include "gtest/gtest.h"
#include <cstddef>

#include "zen/iterator_adaptor.hpp"

using namespace zen;

class next_n_iter : public iterator_adaptor<next_n_iter, int> {
public:

  std::ptrdiff_t value;

  next_n_iter(std::ptrdiff_t value):
    value(value) {}

  unsigned int prev_n_call_count = 0;
  unsigned int next_n_call_count = 0;
  unsigned int increment_call_count = 0;
  unsigned int decrement_call_count = 0;

  void increment() {
    increment_call_count++;
    value++;
  }

  next_n_iter next_n(std::ptrdiff_t n) {
    next_n_call_count++;
    return next_n_iter(value + n);
  }

};

static_assert(has_next_n_v<next_n_iter>);

TEST(IteratorAdaptorTest, CallsNextNWhenPresent) {
  next_n_iter iter(0);
  auto iter_20 = iter + 20;
  ASSERT_EQ(iter.value, 0);
  ASSERT_EQ(iter_20.increment_call_count, 0);
  ASSERT_EQ(iter_20.value, 20);
  ASSERT_EQ(iter.increment_call_count, 0);
  ASSERT_EQ(iter.next_n_call_count, 1);
}

