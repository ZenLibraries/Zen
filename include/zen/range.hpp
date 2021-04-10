#ifndef ZEN_ITERATOR_RANGE_HPP
#define ZEN_ITERATOR_RANGE_HPP

#include <tuple>
#include <type_traits>

#include "zen/zip_iterator.hpp"

namespace zen {

  template<typename IterT>
  class iterator_range {

    IterT left;
    IterT right;

    using trait_type = std::iterator_traits<IterT>;

  public:

    using value_type = typename trait_type::value_type;
    using reference = typename trait_type::reference;
    using iterator = IterT;
    using difference_type = typename trait_type::difference_type;

    iterator_range(IterT left, IterT right):
      left(left), right(right) {}

    IterT begin() {
      return left;
    }

    IterT end() {
      return end;
    }

  };

  template<typename IterT>
  auto make_iterator_range(IterT&& a, IterT&& b) {
    return iterator_range<IterT>(std::forward<IterT>(a), std::forward<IterT>(b));
  }

  template<typename ...Ts>
  struct zip_impl<
    std::tuple<Ts...>
  , std::enable_if_t<meta::andmap_v<meta::lift<meta::is_range>, std::tuple<std::remove_reference_t<Ts>...>>>
  > {
    static auto apply(Ts&& ...args) {
      return make_iterator_range(
          zip(args.begin()...),
          zip(args.end()...)
      );
    }
  };

}

#endif // of #ifndef ZEN_ITERATOR_RANGE_HPP
