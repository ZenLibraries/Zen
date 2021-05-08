
#include "gtest/gtest.h"

#include "zen/encoder.hpp"

using namespace zen;

class Point {
public:

  float x;
  float y;

  void encode(encoder& ec) const {
    auto s = ec.encode_struct("Point");
    s.encode_field("x", x);
    s.encode_field("y", y);
    s.finalize();
  }

};

TEST(EncoderTest, CanEncodeJsonNoIndent) {
  std::vector ps { Point { 1.0, 2.0 }, Point { 3.0, 4.0 }, Point { 5.0, 6.0 } };
  auto str = encode_json(ps);
  ASSERT_EQ(str, "[{\"x\":1.0,\"y\":2.0},{\"x\":3.0,\"y\":4.0},{\"x\":5.0,\"y\":6.0}]");
}

TEST(EncoderTest, CanEncodeJsonWithIndent) {
  std::vector ps { Point { 1.0, 2.0 }, Point { 3.0, 4.0 }, Point { 5.0, 6.0 } };
  auto str = encode_json_pretty(ps);
  ASSERT_EQ(str, "[\n  {\n    \"x\": 1.0,\n    \"y\": 2.0\n  },\n  {\n    \"x\": 3.0,\n    \"y\": 4.0\n  },\n  {\n    \"x\": 5.0,\n    \"y\": 6.0\n  }\n]");
}

