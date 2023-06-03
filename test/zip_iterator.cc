
#include <iostream>
#include <iterator>
#include <type_traits>
#include <unordered_set>
#include <vector>

#include "gtest/gtest.h"

#include "zen/zip_iterator.hpp"
#include "zen/range.hpp"

using namespace zen;

using test_zip_iter_traits = std::iterator_traits<zip_iterator<std::tuple<std::vector<int>>>>;

using zip1 = zip_iterator<std::tuple<std::vector<int>>>;
using zip2 = zip_iterator<std::tuple<std::vector<int>, std::unordered_set<float>>>;

static_assert(std::is_same_v<zip1::iterator_category, std::random_access_iterator_tag>);

static_assert(std::is_same_v<zip_iterator<std::tuple<std::vector<int>>>::value_type, std::tuple<int>>);

TEST(ZipIteratorTest, CanZipPairs) {

  std::vector<int> a { 1, 2, 3 , 4 };
  std::vector<float> b { 1.0, 2.0, 3.0, 4.0 };

  auto zipper = zip(a.begin(), b.begin());

  auto [x1, y1] = *zipper;
  ASSERT_EQ(x1, 1);
  ASSERT_EQ(y1, 1.0);

}

TEST(ZipRangeTest, CanZipPairs) {

  std::vector<int> a { 1, 2, 3 , 4 };
  std::vector<float> b { 1.0, 2.0, 3.0, 4.0 };

  auto zipper = zip(a, b);

  auto iter = zipper.begin();

  auto [x1, y1] = *iter;
  ASSERT_EQ(x1, 1);
  ASSERT_EQ(y1, 1.0);

}

TEST(ZipIteratorTest, CanZipCStrings) {

  const char* a = "foo";
  const char* b = "bar";

  auto zipper = zip(a,b);

}

