
#include <iostream>
#include <vector>

#include "gtest/gtest.h"

#include "zen/zip_iterator.hpp"

using namespace zen;

TEST(ZipIteratorTest, CanZipPairs) {

  std::vector<int> a { 1, 2, 3 , 4 };
  std::vector<float> b { 1.0, 2.0, 3.0, 4.0 };

  auto zipper = zip(a.begin(), b.begin());

  auto [x1, y1] = *zipper;
  ASSERT_EQ(x1, 1);
  ASSERT_EQ(y1, 1.0);

}

