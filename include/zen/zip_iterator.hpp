#ifndef ZEN_ZIP_ITERATOR_HPP
#define ZEN_ZIP_ITERATOR_HPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <tuple>
#include <type_traits>
#include <utility>

#include "zen/meta.hpp"
#include "zen/algorithm.hpp"

ZEN_NAMESPACE_START

template<typename IterT>
struct _get_iterator_category {
  using type = std::iterator_traits<IterT>::iterator_category;
};

using tags = std::tuple<
  std::input_iterator_tag
, std::forward_iterator_tag
, std::bidirectional_iterator_tag
, std::random_access_iterator_tag
, std::output_iterator_tag
>;

template<typename T>
class zip_iterator;

template<typename T>
class zip_iterator {

  T iterators;

public:

  using value_type = meta::map_t<T, meta::lift<meta::get_element<meta::_1>>>;

  /**
   * Because we always return a freshly constructed tuple (an rvalue), we cannot
   * return a reference to something that has no definitive memory location. 
   * Therefore, a reference is always equal to a plain old value.
   */
  using reference = value_type;

  using pointer = value_type*;

  using difference_type = std::ptrdiff_t;

  // TODO Compute the lowest common iterator category from the elements
  using iterator_category = std::random_access_iterator_tag;
  // static_assert(meta::index_t<tags,std::input_iterator_tag>::value == 0);
  // using iterator_category = meta::fold1_t<meta::lift<meta::min_by_t<meta::lift<meta::index_t<tags, meta::_1>>, std::tuple<meta::_1, _get_iterator_category<meta::_2>>, std::input_iterator_tag>>, T>;

  zip_iterator(T iterators):
    iterators(iterators) {}

  zip_iterator(const zip_iterator& other):
    iterators(other.iterators) {}

  zip_iterator(zip_iterator&& other):
    iterators(std::move(other.iterators)) {}

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

  void operator++() {
    std::apply([&](auto& ...args) { ((++args),...); }, iterators);
  }

  void operator--() {
    std::apply([&](auto& ...args) { ((--args),...); }, iterators);
  }

  zip_iterator operator+(std::ptrdiff_t offset) {
    return convert(iterators, [&] (auto& iter) { return iter + offset; });
  }

  zip_iterator operator-(std::ptrdiff_t offset) {
    return convert(iterators, [&] (auto& iter) { return iter - offset; });
  }

  zip_iterator& operator=(const zip_iterator& other) {
    iterators = other.iterators;
    return *this;
  }

  reference operator*() {
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

namespace std {

  template <typename T>
  struct iterator_traits<zen::zip_iterator<T>>
  {
      using difference_type = ZEN_NAMESPACE::zip_iterator<T>::difference_type;
      using value_type = ZEN_NAMESPACE::zip_iterator<T>::value_type;
      using pointer = ZEN_NAMESPACE::zip_iterator<T>::pointer;
      using reference = ZEN_NAMESPACE::zip_iterator<T>::reference;
      using iterator_category = ZEN_NAMESPACE::zip_iterator<T>::iterator_category;
  };

}

#endif // #ifndef ZEN_ZIP_ITERATOR_HPP
