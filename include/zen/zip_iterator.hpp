#ifndef ZEN_ZIP_ITERATOR_HPP
#define ZEN_ZIP_ITERATOR_HPP

#include <algorithm>
#include <cstdint>
#include <tuple>
#include <type_traits>
#include <utility>

#include "zen/meta.hpp"
#include "zen/iterator_adaptor.hpp"

ZEN_NAMESPACE_START

template<typename T>
class zip_iterator;

template<typename T>
class zip_iterator : public iterator_adaptor<
    zip_iterator<T>,
    meta::map_t<T, meta::lift<meta::get_element<meta::_1>>>,
    meta::map_t<T, meta::lift<meta::get_element<meta::_1>>>
 > {

  T iterators;

public:

  using value_type = meta::map_t<T, meta::lift<meta::get_element<meta::_1>>>;
  using reference = value_type;

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

  bool operator==(const zip_iterator& other) const {
    return std::get<0>(iterators) == std::get<0>(other.iterators);
  }

  bool operator!=(const zip_iterator& other) const {
    return std::get<0>(iterators) != std::get<0>(other.iterators);
  }

  void increment() {
    std::apply([&](auto& ...args) { ((++args),...); }, iterators);
  }

  void decrement() {
    std::apply([&](auto& ...args) { ((--args),...); }, iterators);
  }

  zip_iterator next_n(std::ptrdiff_t offset) {
    return convert(iterators, [&] (auto& iter) { return iter + offset; });
  }

  reference dereference() {
    return convert(iterators, [&] (const auto& iter) { return *iter; });
  }

};

template<typename ...Ts>
struct zip_impl<
  std::tuple<Ts...>
, std::enable_if_t<meta::andmap_v<meta::lift<meta::is_iterator<std::remove_reference<meta::_1>>>, std::tuple<Ts...>>>
> {
  static auto apply(Ts&& ...args) {
    return zip_iterator<std::tuple<Ts...>>(std::tuple<Ts...>(std::forward<Ts>(args)...));
  }
};

ZEN_NAMESPACE_END

#endif // #ifndef ZEN_ZIP_ITERATOR_HPP
