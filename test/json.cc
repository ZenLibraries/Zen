
#include <string>

#include "gtest/gtest.h"

#include "zen/json.hpp"

// TODO Simplify these tests by using Unicode-style string literals.

TEST(JsonParse, CanParseSimpleStringLiteral) {
  auto r1 = zen::parse_json("\"Hello, world!\"").unwrap();
  ASSERT_TRUE(r1.is_string());
  auto s1 = r1.as_string();
  ASSERT_EQ(s1.size(), 13);
  ASSERT_EQ(s1[0], 'H');
  ASSERT_EQ(s1[1], 'e');
  ASSERT_EQ(s1[2], 'l');
  ASSERT_EQ(s1[3], 'l');
  ASSERT_EQ(s1[4], 'o');
  ASSERT_EQ(s1[5], ',');
  ASSERT_EQ(s1[6], ' ');
  ASSERT_EQ(s1[7], 'w');
  ASSERT_EQ(s1[8], 'o');
  ASSERT_EQ(s1[9], 'r');
  ASSERT_EQ(s1[10], 'l');
  ASSERT_EQ(s1[11], 'd');
  ASSERT_EQ(s1[12], '!');
}

TEST(JsonParse, CanParseASCIIEscapeSequence) {
  auto r1 = zen::parse_json("\"\\\"\\\\\\/\\b\\f\\n\\r\\t\"").unwrap();
  ASSERT_TRUE(r1.is_string());
  auto s1 = r1.as_string();
  ASSERT_EQ(s1.size(), 8);
  ASSERT_EQ(s1[0], '\"');
  ASSERT_EQ(s1[1], '\\');
  ASSERT_EQ(s1[2], '/');
  ASSERT_EQ(s1[3], '\b');
  ASSERT_EQ(s1[4], '\f');
  ASSERT_EQ(s1[5], '\n');
  ASSERT_EQ(s1[6], '\r');
  ASSERT_EQ(s1[7], '\t');
}

TEST(JsonParse, CanParseSimpleInteger) {
  auto r1 = zen::parse_json("1").unwrap();
  ASSERT_TRUE(r1.is_integer());
  ASSERT_EQ(r1.as_integer(), 1);
  auto r2 = zen::parse_json("1234567890").unwrap();
  ASSERT_TRUE(r2.is_integer());
  ASSERT_EQ(r2.as_integer(), 1234567890);
}

TEST(JsonParse, CanParseArrayListWithWhiteSpace) {
  auto r1 = zen::parse_json(" [ 1 , 2 , 3 ] ").unwrap();
  ASSERT_TRUE(r1.is_array());
  auto& a1 = r1.as_array();
  ASSERT_EQ(a1.size(), 3);
  ASSERT_TRUE(a1[0].is_integer());
  ASSERT_TRUE(a1[1].is_integer());
  ASSERT_TRUE(a1[2].is_integer());
  ASSERT_EQ(a1[0].as_integer(), 1);
  ASSERT_EQ(a1[1].as_integer(), 2);
  ASSERT_EQ(a1[2].as_integer(), 3);
}

TEST(JsonParse, CanParseNull) {
  auto r1 = zen::parse_json("null").unwrap();
  ASSERT_TRUE(r1.is_null());
}

TEST(JsonParse, CanParseBool) {
  auto r1 = zen::parse_json("true").unwrap();
  ASSERT_TRUE(r1.is_boolean());
  ASSERT_EQ(r1.as_boolean(), true);
  auto r2 = zen::parse_json("false").unwrap();
  ASSERT_TRUE(r2.is_boolean());
  ASSERT_EQ(r2.as_boolean(), false);
}

TEST(JsonParse, CanParseEmptyObject) {
  auto r1 = zen::parse_json("{}").unwrap();
  ASSERT_TRUE(r1.is_object());
  ASSERT_EQ(r1.as_object().size(), 0);
}

TEST(JsonParse, CanParseSimpleObjectWitWhiteSpace) {
  auto r1 = zen::parse_json(" { \"foo\" : 1 , \"bar\" : 2 } ").unwrap();
  ASSERT_TRUE(r1.is_object());
  auto& o1 = r1.as_object();
  ASSERT_EQ(o1.size(), 2);
  // ASSERT_EQ(o1["foo"], 1);
  // ASSERT_EQ(o1["bar"], 2);
}

TEST(JsonParse, CanParseSimpleObject) {
  auto r1 = zen::parse_json("{\"foo\":1,\"bar\":2}").unwrap();
  ASSERT_TRUE(r1.is_object());
  auto& o1 = r1.as_object();
  ASSERT_EQ(o1.size(), 2);
  // ASSERT_EQ(o1["foo"], 1);
  // ASSERT_EQ(o1["bar"], 2);
}

TEST(JsonParse, DoesNotCrashWhenGivenInvalidChars) {
  auto r1 = zen::parse_json("@");
  ASSERT_TRUE(r1.is_left());
  auto r2 = zen::parse_json(" [ 1 @ ]");
  ASSERT_TRUE(r2.is_left());
  auto r3 = zen::parse_json(" [ 1 , @ ]");
  ASSERT_TRUE(r3.is_left());
  auto r4 = zen::parse_json(",");
  ASSERT_TRUE(r4.is_left());
  auto r5 = zen::parse_json("#");
  ASSERT_TRUE(r5.is_left());
  auto r6 = zen::parse_json("&");
  ASSERT_TRUE(r6.is_left());
}

TEST(JsonParse, CanParseFractional) {
  auto r1 = zen::parse_json("2.3").unwrap();
  ASSERT_TRUE(r1.is_fractional());
  ASSERT_EQ(r1.as_fractional(), 2.3);
}
