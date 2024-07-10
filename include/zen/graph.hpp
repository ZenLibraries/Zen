#ifndef ZEN_GRAPH_HPP
#define ZEN_GRAPH_HPP

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <stack>
#include <optional>

#include "zen/range.hpp"

ZEN_NAMESPACE_START

struct no_label_t {};

template<typename V, typename L = no_label_t>
class hash_graph {
private:

  struct in_edge_t {
    V vertex;
    L label;
  };

  struct out_edge_t {
    L label;
    V vertex;
  };

  std::unordered_set<V> vertices;

  std::unordered_multimap<V, out_edge_t> out_edges;

public:

  using vertex_type = V;
  using label_type = L;

  void add_vertex(V vertex) {
    vertices.emplace(vertex);
  }

  void add_edge(V from, V to, L label) {
    out_edges.emplace(from, out_edge_t { label, to });
  }

  void add_edge(V from, V to) requires (std::is_same_v<L, no_label_t>) {
    out_edges.emplace(from, out_edge_t { {}, to });
  }

  std::vector<V> get_target_vertices(const V& from) const {
    std::vector<V> out;
    for (auto [_, edge]: make_iterator_range(out_edges.equal_range(from))) {
      out.push_back(edge.vertex);
    }
    return out;
  }

  std::size_t count_vertices() {
    return vertices.size();
  }

  auto get_vertices() {
    return make_iterator_range(vertices.begin(), vertices.end());
  }

  auto get_vertices() const {
    return make_iterator_range(vertices.cbegin(), vertices.cend());
  }

};

// template<typename V>
// class hash_graph<V, no_label_t> {
// public:

//   using in_edge_t = V;
//   using out_edge_t = V;

//   using vertex_type = V;
//   using label_type = no_label_t;

// private:

//   std::unordered_set<V> vertices;

//   std::unordered_multimap<V, out_edge_t> out_edges;

// public:

//   void add_vertex(V vertex) {
//     vertices.emplace(vertex);
//   }

//   void add_edge(V from, V to) {
//     out_edges.emplace(from, to);
//   }

//   std::vector<out_edge_t> get_out_edges(const V& from) {
//     std::vector<out_edge_t> out;
//     for (auto& x: out_edges.equal_range(from)) {
//       out.push_back(x);
//     }
//     return out;
//   }
 
// };

template<typename G>
std::vector<typename G::vertex_type> dijkstra(const G& graph) {
  // TODO
}

template<typename Graph>
std::vector<std::vector<typename Graph::vertex_type>> toposort(const Graph& graph) {

  using V = typename Graph::vertex_type;

  struct TarjanVertexData {
    std::optional<std::size_t> index;
    std::size_t low_link;
    bool on_stack = false;
  };

  class TarjanSolver {
  public:

    std::vector<std::vector<V>> components;

  private:

    const Graph& graph;
    std::unordered_map<V, TarjanVertexData> mapping;
    std::size_t index = 0;
    std::stack<V> stack;

    TarjanVertexData& get_data(V from) {
      return mapping.emplace(from, TarjanVertexData {}).first->second;
    }

    void visit_cycle(const V& from) {

      auto& data_from = get_data(from);
      data_from.index = index;
      data_from.low_link = index;
      index++;
      stack.push(from);
      data_from.on_stack = true;

      for (const auto& to: graph.get_target_vertices(from)) {
        auto& data_to = get_data(to);
        if (!data_to.index) {
          visit_cycle(to);
          data_from.low_link = std::min(data_from.low_link, data_to.low_link);
        } else if (data_to.on_stack) {
          data_from.low_link = std::min(data_from.low_link, *data_to.index);
        }
      }

      if (data_from.low_link == data_from.index) {
        std::vector<V> component;
        for (;;) {
          auto& X = stack.top();
          stack.pop();
          auto& data_x = get_data(X);
          data_x.on_stack = false;
          component.push_back(X);
          if (X == from) {
            break;
          }
        }
        components.push_back(component);
      }

    }

   public:

     TarjanSolver(const Graph& graph):
       graph(graph) {}

     void solve() {
       for (auto from: graph.get_vertices()) {
         if (!mapping.count(from)) {
           visit_cycle(from);
         }
       }
     }

  };

  TarjanSolver S { graph };
  S.solve();
  return S.components;

}

ZEN_NAMESPACE_END

#endif // of #ifndef ZEN_GRAPH_HPP
