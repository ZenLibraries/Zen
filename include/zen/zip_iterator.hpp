#ifndef ZEN_ZIP_ITERATOR_HPP
#define ZEN_ZIP_ITERATOR_HPP

#include <cstdint>
#include <type_traits>
#include <utility>

#include "zen/meta.hpp"
#include "zen/iterator_adaptor.hpp"

namespace zen {

  template<typename T>
  class zip_iterator;

  template<typename T>
  class zip_iterator : public iterator_adaptor<
      zip_iterator<T>,
      meta::map_t<T, meta::lift<meta::element>>,
      meta::map_t<T, meta::lift<meta::element>>
   > {

    T iterators;

    using ValueT = meta::map_t<T, meta::lift<meta::element>>;
    using ReferenceT = ValueT;

  public:

    zip_iterator(T iterators):
      iterators(iterators) {}

    zip_iterator(const zip_iterator& other):
      iterators(other.iterators) {}

    zip_iterator(zip_iterator&& other):
      iterators(std::move(other.iterators)) {}

    zip_iterator& operator=(const zip_iterator& other) {
      iterators = other.iterators;
      return *this;
    }

    zip_iterator& operator=(zip_iterator&& other) {
      iterators = std::move(other.iterators);
      return *this;
    }

    void increment() {
      for (auto& iter: iterators) {
        ++iter;
      }
    }

    void decrement() {
      for (auto& iter: iterators) {
        --iter;
      }
    }

    zip_iterator next_n(std::ptrdiff_t offset) {
      return convert(iterators, [&] (auto& iter) { return iter + offset; });
    }

    ReferenceT dereference() {
      return convert(iterators, [&] (const auto& iter) { return *iter; });
    }

  };

  template<typename ...Ts>
  struct zip_impl<
    std::tuple<Ts...>
  , std::enable_if_t<meta::andmap_v<meta::lift<meta::is_iterator>, std::tuple<Ts...>>>
  > {
    static auto apply(Ts&& ...args) {
      return zip_iterator<std::tuple<Ts...>>(std::tuple<Ts...>(std::forward<Ts>(args)...));
    }
  };

}

#endif // #ifndef ZEN_ZIP_ITERATOR_HPP
