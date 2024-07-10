
#include "gtest/gtest.h"

#include "zen/graph.hpp"

TEST(GraphTest, TestOutEdges) {
  zen::hash_graph<int> g;
  g.add_edge(1, 2);
  g.add_edge(1, 4);
  g.add_edge(2, 3);
  g.add_edge(3, 1);
  auto targets_1 = g.get_target_vertices(1);
  std::map<int, int> m;
  for (int i = 1; i <= 4; i++) {
    m[i] = 0;
  }
  for (auto v: targets_1) {
    ++m[v];
  }
  ASSERT_EQ(m[1], 0);
  ASSERT_EQ(m[2], 1);
  ASSERT_EQ(m[3], 0);
  ASSERT_EQ(m[4], 1);
}
