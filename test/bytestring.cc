
#include "gtest/gtest.h"

#include "zen/bytestring.hpp"
#include "zen/meta.hpp"

static_assert(zen::meta::is_range_v<const zen::basic_bytestring<>>);
static_assert(zen::meta::is_range_v<const zen::bytestring_view>);

TEST(ByteStringTest, CanConstructFromStringLiteral) {
  zen::bytestring a = "foo";
  zen::bytestring b { "bar" };
}

TEST(ByteStingTest, CanCheckEqualityBetweenTwoByteStrings) {

  zen::bytestring a { "foo" };
  zen::bytestring b { "bar" };
  zen::bytestring c { "foo" };
  zen::bytestring d { "foob" };
  zen::bytestring e;

  ASSERT_TRUE(a == a);
  ASSERT_FALSE(a == b);
  ASSERT_TRUE(a == c);
  ASSERT_FALSE(a == d);
  ASSERT_FALSE(b == a);
  ASSERT_TRUE(b == b);
  ASSERT_FALSE(b == c);
  ASSERT_FALSE(b == d);
  ASSERT_TRUE(c == a);
  ASSERT_FALSE(c == b);
  ASSERT_TRUE(c == c);
  ASSERT_FALSE(c == d);
  ASSERT_FALSE(d == a);
  ASSERT_FALSE(d == b);
  ASSERT_FALSE(d == c);
  ASSERT_TRUE(d == d);
}

TEST(ByteStringTest, ReportsCorrectSize) {
  zen::bytestring a { "foo" };
  zen::bytestring b { "" };
  zen::bytestring c;
  zen::bytestring d { "baad" };
  ASSERT_EQ(a.size(), 3);
  ASSERT_EQ(b.size(), 0);
  ASSERT_EQ(c.size(), 0);
  ASSERT_EQ(d.size(), 4);
}

TEST(ByteStringView, CanCompareWithByteString) {

  zen::bytestring a { "foo" };
  auto a_view = a.as_view();
  zen::bytestring b { "ba_viewr" };
  zen::bytestring c { "foo" };
  zen::bytestring d { "foob" };
  zen::bytestring e;

  ASSERT_TRUE(a_view == a);
  ASSERT_FALSE(a_view == b);
  ASSERT_TRUE(a_view == c);
  ASSERT_FALSE(a_view == d);
  ASSERT_FALSE(b == a_view);
  ASSERT_TRUE(b == b);
  ASSERT_FALSE(b == c);
  ASSERT_FALSE(b == d);
  ASSERT_TRUE(c == a_view);
  ASSERT_FALSE(c == b);
  ASSERT_TRUE(c == c);
  ASSERT_FALSE(c == d);
  ASSERT_FALSE(d == a_view);
  ASSERT_FALSE(d == b);
  ASSERT_FALSE(d == c);
  ASSERT_TRUE(d == d);

}

