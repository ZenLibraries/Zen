
#include "gtest/gtest.h"

#include "zen/fs/filepath.hpp"

TEST(FilePathTest, CanNormalizePath) {
  zen::fs::filepath p1 = "././../CMakeLists.txt";
  p1.normalize();
  ASSERT_EQ(p1, "../CMakeLists.txt");
  zen::fs::filepath p2 = "../foo/../../CMakeLists.txt";
  p2.normalize();
  ASSERT_EQ(p2, "../foo/../../CMakeLists.txt");
}

