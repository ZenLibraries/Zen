
#include "gtest/gtest.h"

#include "zen/unicode.hpp"

using zen::operator""_utf8;

TEST(UTF8Decode, CanDecodeASCIIChars) {
  auto str = "abcde01234"_utf8;
  ASSERT_EQ(str[0], 'a');
  ASSERT_EQ(str[1], 'b');
  ASSERT_EQ(str[2], 'c');
  ASSERT_EQ(str[3], 'd');
  ASSERT_EQ(str[4], 'e');
  ASSERT_EQ(str[5], '0');
  ASSERT_EQ(str[6], '1');
  ASSERT_EQ(str[7], '2');
  ASSERT_EQ(str[8], '3');
  ASSERT_EQ(str[9], '4');
}
