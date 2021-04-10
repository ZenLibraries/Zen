#ifndef ZEN_ZIP_ITERATOR_HPP
#define ZEN_ZIP_ITERATOR_HPP

#include <cstdint>
#include <type_traits>
#include <utility>

#include "zen/meta.hpp"

namespace zen {

  // template<typename T, typename FnT>
  // std::invoke_result_t<FnT, meta::element_t<T>> convert(const T& value, FnT op);

#define ZEN_CONSTEXPR constexpr

  template<typename FnT, std::size_t ...J, typename ...Ts>
  ZEN_CONSTEXPR auto convert_impl(const std::tuple<Ts...>& value, std::integer_sequence<std::size_t, J...>, FnT&& op) {
    return std::tuple<std::invoke_result_t<FnT, Ts>...> { op(std::get<J>(value))... };
  }

  template<typename FnT, typename ...Ts>
  ZEN_CONSTEXPR auto convert(const std::tuple<Ts...>& value, FnT op) {
    using seq = std::make_index_sequence<std::tuple_size_v<std::tuple<Ts...>>>;
    return convert_impl(std::tuple<Ts...>(value), seq(), std::forward<FnT>(op));
  }

  template<typename T1, typename T2, typename FnT>
  ZEN_CONSTEXPR auto convert(const std::pair<T1, T2>& value, FnT fn) {
    return std::make_pair<T1, T2>(fn(value.first), fn(value.second));
  }

  template<typename T>
  class zip_iterator {

    T iterators;

  public:

    using value_type = meta::map_t<T, meta::lift<meta::element>>;

    zip_iterator(T iterators):
      iterators(iterators) {}

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

    value_type derefence() {
      return convert(iterators, [&] (const auto& iter) { return *iter; });
    }

  };

  template<typename ...Ts>
  auto zip(Ts...args) {
    return zip_iterator<std::tuple<Ts...>>(std::tuple<Ts...>(args...));
  }

}

#endif // #ifndef ZEN_ZIP_ITERATOR_HPP
