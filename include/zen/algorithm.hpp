#ifndef ZEN_ALGORITHM_HPP
#define ZEN_ALGORITHM_HPP

#include "zen/meta.hpp"
#include <iostream>

namespace zen {

  template<typename T>
  auto decrement(T& value) {
    --value;
  }

  template<typename T>
  auto increment(T& value) {
    ++value;
  }

  template<typename T>
  T prev_n(T value, meta::difference_t<T> n) {
    auto curr = value;
    for (std::ptrdiff_t i = 0; i < n; i++) {
      decrement(curr);
    }
    return curr;
  }

  template<typename T, typename = void>
  struct next_n_impl {
    static T apply(T& value, meta::difference_t<T> n) {
      auto curr = value;
      for (std::ptrdiff_t i = 0; i < n; i++) {
        increment(curr);
      }
      return curr;
    }
  };

  template<typename T>
  T next_n(T& value, meta::difference_t<T> n) {
    return next_n_impl<T>::apply(value, n);
  }

}

#endif // of #ifndef ZEN_ALGORITHM_HPP
