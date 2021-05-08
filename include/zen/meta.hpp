#ifndef ZEN_META_HPP
#define ZEN_META_HPP

#include <cstddef>
#include <iterator>
#include <tuple>
#include <type_traits>
#include <memory>

ZEN_NAMESPACE_START

namespace meta {

  template<typename T, typename F>
  struct map;

  template<typename ...Ts, typename F>
  struct map<std::tuple<Ts...>, F> {
    using type = std::tuple<typename F::template apply<Ts>::type...>;
  };

  template<typename T, typename F>
  using map_t = typename map<T, F>::type;

  template<template <typename ...> class F>
  struct lift {
    template<typename ...Ts>
    using apply = F<Ts...>;
  };

  template<typename T>
  struct element {
    using type = typename std::remove_reference_t<T>::value_type;
  };

  template<typename T>
  using element_t = typename element<T>::type;

  template<typename T>
  struct element_reference {
    using type = typename T::reference_type;
  };

  template<typename T>
  using element_reference_t = typename element_reference<T>::type;

  /// Calculate the type that is used to represent the difference between two
  /// instances of the given type.
  template<typename T>
  struct difference {
    using type = typename std::remove_reference_t<T>::difference_type;
  };

  template<typename T>
  using difference_t = typename difference<T>::type;

  template<typename T>
  struct difference<T*> {
    using type = std::ptrdiff_t;
  };

  template <typename T, typename = void>
  struct is_std_container : std::false_type { };

  template <typename T>
  struct is_std_container<T,
      std::void_t<
        decltype(std::declval<T&>().begin()),
        decltype(std::declval<T&>().end()),
        typename T::value_type
      >
    > : std::true_type { };

  template <typename T, typename = void>
  struct is_range : std::false_type { };

  template <typename T>
  struct is_range<T,
      std::void_t<
        decltype(std::declval<T&>().begin()),
        decltype(std::declval<T&>().end()),
        typename T::value_type
      >
    > : std::true_type { };

  template<typename T>
  constexpr const bool is_range_v = is_range<T>::value;

  template<std::size_t N, typename FnT, typename ...Ts>
  struct andmap_impl;

  template<typename FnT>
  struct andmap_impl<0, FnT> {
    static constexpr const bool value = true;
  };

  template<std::size_t N, typename FnT, typename T, typename ...Ts>
  struct andmap_impl<N, FnT, T, Ts...> {
    static constexpr const bool value = FnT::template apply<T>::value && andmap_impl<N-1, FnT, Ts...>::value;
  };

  template<typename FnT, typename T>
  struct andmap;

  template<typename FnT, typename ...Ts>
  struct andmap<FnT, std::tuple<Ts...>> {
    static constexpr const bool value = andmap_impl<std::tuple_size_v<std::tuple<Ts...>>, FnT, Ts...>::value;
  };

  template<typename FnT, typename T>
  constexpr const bool andmap_v = andmap<FnT, T>::value;

  template <typename T, typename = void>
  struct is_iterator : std::false_type { };

  template <typename T>
  struct is_iterator<T,
      std::void_t< typename std::iterator_traits<T>::value_type >
    > : std::true_type { };


  template<typename T>
  struct is_pointer : std::false_type {};

  template<typename T>
  struct is_pointer<T*> : std::true_type {};

  template<typename T>
  struct is_pointer<std::shared_ptr<T>> : std::true_type {};

  template<typename T>
  struct is_pointer<std::unique_ptr<T>> : std::true_type {};

  template<typename T>
  static constexpr const bool is_pointer_v = is_pointer<T>::value;

  template<typename T>
  struct pointer_element;

  template<typename T>
  struct pointer_element<T*> {
    using type = T;
  };

  template<typename T>
  struct pointer_element<std::shared_ptr<T>> {
    using type = T;
  };

  template<typename T>
  struct pointer_element<std::unique_ptr<T>> {
    using type = T;
  };

  template<typename T>
  using pointer_element_t = typename pointer_element<T>::type;

  template<typename T, typename = void>
  struct is_container : std::false_type {};

  template<typename T>
  struct is_container<T,
      std::void_t<
        decltype(std::declval<T&>().begin()),
        decltype(std::declval<T&>().end()),
        typename T::value_type,
        typename T::iterator
        >
      > : std::true_type { };

  template<typename T>
  static constexpr const bool is_container_v = is_container<T>::value;

}

ZEN_NAMESPACE_END

#endif // of #ifndef ZEN_META_HPP
