#ifndef ZEN_META_HPP
#define ZEN_META_HPP

#include <cstddef>
#include <iterator>
#include <tuple>
#include <type_traits>
#include <memory>

ZEN_NAMESPACE_START

namespace meta {

  struct _ {};
  struct _1 {};
  struct _2 {};
  struct _3 {};
  struct _4 {};
  struct _5 {};
  struct _6 {};
  struct _7 {};
  struct _8 {};
  struct _9 {};

  template<std::size_t I>
  using sz = std::integral_constant<std::size_t, I>;

  using u0 = std::integral_constant<unsigned, 0>;
  using u1 = std::integral_constant<unsigned, 1>;
  using u2 = std::integral_constant<unsigned, 2>;
  using u3 = std::integral_constant<unsigned, 3>;
  using u4 = std::integral_constant<unsigned, 4>;
  using u5 = std::integral_constant<unsigned, 5>;
  using u6 = std::integral_constant<unsigned, 8>;
  using u7 = std::integral_constant<unsigned, 7>;
  using u8 = std::integral_constant<unsigned, 8>;
  using u9 = std::integral_constant<unsigned, 9>;

  template<typename T1, typename T2>
  struct less;

  template<typename I, I T1, I T2>
  struct less<std::integral_constant<I, T1>, std::integral_constant<I, T2>> {
    static constexpr const bool value = T1 < T2;
  };

  template<typename I, typename T>
  struct get;

  template<typename I, typename ...Ts>
  struct get<I, std::tuple<Ts...>> {
    using type = std::tuple_element_t<I::value, std::tuple<Ts...>>;
  };

  template<typename T1, typename T2>
  struct get<u0, std::pair<T1, T2>> {
    using type = T1;
  };

  template<typename T1, typename T2>
  struct get<u1, std::pair<T1, T2>> {
    using type = T2;
  };

  template<typename T>
  struct head;

  template<typename ...Ts>
  struct head<std::tuple<Ts...>> {
    using type = std::tuple_element_t<0, std::tuple<Ts...>>;
  };

  template<typename T1, typename T2>
  struct head<std::pair<T1, T2>> {
    using type = T1;
  };

  template<typename T>
  using head_t = typename head<T>::type;

  template<typename T>
  struct rest;

  template<typename T1, typename ...Ts>
  struct rest<std::tuple<T1, Ts...>> {
    using type = std::tuple<Ts...>;
  };

  template<typename T>
  using rest_t = typename rest<T>::type;

  template<typename T>
  struct is_null;

  template<>
  struct is_null<std::tuple<>> : std::true_type {};

  template<typename T1, typename ...Ts>
  struct is_null<std::tuple<T1, Ts...>> : std::false_type {};

  template<typename T>
  using is_null_v = is_null<T>::value;

  template<typename FnT, typename ...Ts>
  struct apply {
    using type = FnT::template apply<Ts...>;
  };

  template<typename FnT, typename ...Ts>
  using apply_t = apply<FnT, Ts...>::type;

  template<typename T, typename F>
  struct map;

  template<typename ...Ts, typename F>
  struct map<std::tuple<Ts...>, F> {
    using type = std::tuple<apply_t<F, Ts>...>;
  };

  template<typename T, typename F>
  using map_t = typename map<T, F>::type;

  template<typename KeyT, typename T1, typename ...Ts>
  struct assoc_helper {
    using type = std::conditional_t<
      std::is_same_v<typename T1::first_type, KeyT>,
      typename T1::second_type,
      typename assoc_helper<KeyT, Ts...>::type
    >;
  };

  template<typename KeyT, typename ListT>
  struct assoc;

  template<typename KeyT, typename ...Ts>
  struct assoc<KeyT, std::tuple<Ts...>> {
    using type = typename assoc_helper<KeyT, Ts...>::type;
  };

  template<typename Env, typename T>
  struct eval {
    using type = T;
  };

  template<typename Env, typename T>
  using eval_t = typename eval<Env, T>::type;

  template<typename Env>
  struct eval<Env, _1> {
    using type = std::tuple_element_t<0, Env>;
  };

  template<typename Env>
  struct eval<Env, _2> {
    using type = std::tuple_element_t<1, Env>;
  };

  template<typename Env>
  struct eval<Env, _3> {
    using type = std::tuple_element_t<2, Env>;
  };

  template<typename Env>
  struct eval<Env, _4> {
    using type = std::tuple_element_t<3, Env>;
  };

  template<typename Env>
  struct eval<Env, _5> {
    using type = std::tuple_element_t<4, Env>;
  };

  template<typename Env>
  struct eval<Env, _6> {
    using type = std::tuple_element_t<5, Env>;
  };

  template<typename Env>
  struct eval<Env, _7> {
    using type = std::tuple_element_t<6, Env>;
  };

  template<typename Env>
  struct eval<Env, _8> {
    using type = std::tuple_element_t<7, Env>;
  };

  template<typename Env>
  struct eval<Env, _9> {
    using type = std::tuple_element_t<8, Env>;
  };

  template<typename Env, template <typename ...> class F, typename ...Ts>
  struct eval<Env, F<Ts...>> {
    using type = typename F<eval_t<Env, Ts>...>::type;
  };

  template<typename T>
  struct lift {
    template<typename ...Ts>
    using apply = eval_t<std::tuple<Ts...>, T>;
  };

  template<typename T, typename = void>
  struct get_element {
    using type = typename std::remove_reference_t<T>::value_type;
  };

  template<typename T>
  struct get_element<T*> {
    using type = T;
  };

  template<typename T>
  struct get_element<T*&> {
    using type = T;
  };

  template<typename T>
  struct get_element<T*&&> {
    using type = T;
  };

  template<typename T>
  using get_element_t = typename get_element<T>::type;

  template<typename T>
  struct get_element_reference {
    using type = typename T::reference_type;
  };

  template<typename T>
  using get_element_reference_t = typename get_element_reference<T>::type;

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
  struct is_nonconst_range : std::false_type { };

  template <typename T>
  struct is_nonconst_range<T,
      std::void_t<
        decltype(std::declval<T&>().begin()),
        decltype(std::declval<T&>().end()),
        typename T::value_type
      >
    > : std::true_type { };

  template <typename T, typename = void>
  struct is_const_range : std::false_type { };

  template <typename T>
  struct is_const_range<T,
      std::void_t<
        decltype(std::declval<const T&>().cbegin()),
        decltype(std::declval<const T&>().cend()),
        const typename T::value_type
      >
    > : std::true_type { };

  template <typename T, typename = void>
  struct is_range : std::conditional_t<
      std::is_const_v<T>,
      is_const_range<T>,
      is_nonconst_range<T>
    > {};

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

  static_assert(is_null<std::tuple<>>::value);

  template<typename FnT, typename T, typename InitT>
  struct fold : std::conditional_t<is_null<T>::value, InitT, apply_t<FnT, head_t<T>, fold<FnT, rest_t<T>, InitT>>> {};

  template<typename FnT, typename T, typename InitT>
  using fold_t = typename fold<FnT, T, InitT>::type;

  template<typename FnT, typename T>
  struct fold1 : fold<FnT, rest_t<T>, head_t<T>> {};

  template<typename FnT, typename T>
  using fold1_t = typename fold1<FnT, T>::type;

  template<typename I1, typename I2>
  struct add;

  template<typename I, I K, I L>
  struct add<std::integral_constant<I, K>, std::integral_constant<I, L>> {
    using type = std::integral_constant<I, K+L>;
  };

  template<typename I1, typename I2>
  using add_t = add<I1, I2>::type;

  template<typename FnT, typename ListT, typename InitT>
  struct min_by : fold<lift<std::conditional_t<apply_t<FnT, _1>::type::value < apply_t<FnT, _2>::value, _1, _2>>, ListT, InitT> {};

  template<typename FnT, typename ListT, typename InitT>
  using min_by_t = typename min_by<FnT, ListT, InitT>::type;

  template<typename ListT, typename NeedleT, std::size_t I = 0>
  struct index {
    using type = std::conditional_t<std::is_same<NeedleT, head_t<ListT>>::value, sz<I>, index<rest_t<ListT>, NeedleT, I + 1>>;
  };

  template<typename ListT, typename NeedleT, std::size_t I = 0>
  using index_t = typename index<ListT, NeedleT, I>::type;

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

  template <typename T, typename = void>
  struct is_iterator : std::false_type { };

  template <typename T>
  struct is_iterator<T,
      std::void_t< typename std::iterator_traits<T>::value_type >
    > : std::true_type { };

  template<typename T>
  static constexpr const bool is_iterator_v = is_iterator<T>::value;

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

  template<typename T, typename U>
  auto static_pointer_cast(U& ptr);

}

ZEN_NAMESPACE_END

#endif // of #ifndef ZEN_META_HPP
