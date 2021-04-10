#ifndef ZEN_META_HPP
#define ZEN_META_HPP

#include <iterator>
#include <tuple>

namespace zen {

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
      using type = typename T::value_type;
    };

    template<typename T>
    using element_t = typename element<T>::type;

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

    template<typename FnT, typename T>
    static constexpr const bool andmap_v = andmap<FnT, T>::value;

    template<typename FnT, typename ...Ts>
    struct andmap<FnT, std::tuple<Ts...>> {
      static constexpr const bool value = andmap_impl<std::tuple_size_v<std::tuple<Ts...>>, FnT, Ts...>::value;
    };

    template <typename T, typename = void>
    struct is_iterator : std::false_type { };

    template <typename T>
    struct is_iterator<T,
        std::void_t< typename std::iterator_traits<T>::value_type >
      > : std::true_type { };

  }

}

#endif // of #ifndef ZEN_META_HPP
