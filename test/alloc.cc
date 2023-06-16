
#include "gtest/gtest.h"

#include "zen/alloc.hpp"

TEST(AllocTest, CanAllocSingleBytes) {
  zen::pool_alloc a;
  for (size_t i = 0; i < 256 * 1024; i++) {
    char* byte = static_cast<char*>(a.allocate(8));
    ASSERT_TRUE(byte);
    *byte = 42;
    ASSERT_EQ(*byte, 42);
  }
}

TEST(AllocTest, FailsToAllocTooLarge) {
  zen::pool_alloc a(1024);
  ASSERT_FALSE(a.allocate(2048));
}

