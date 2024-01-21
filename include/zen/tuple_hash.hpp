
#pragma once

#include <tuple>
#include <functional>

ZEN_NAMESPACE_START

inline std::size_t hash_combiner(std::size_t left, std::size_t right) {
  return left + 0x9e3779b9 + (right<<6) + (right>>2);
}

template<int I, class...Ts>
struct tuple_hash_impl {
    std::size_t operator()(std::size_t a, const std::tuple<Ts...>& t) const {
        typedef typename std::tuple_element<I, std::tuple<Ts...>>::type NextT;
        tuple_hash_impl<I-1, Ts...> next;
        std::size_t b = std::hash<NextT>()(std::get<I>(t));
        return next(hash_combiner(a, b), t);
    }
};

template<class...Ts>
struct tuple_hash_impl<0, Ts...> {
    std::size_t operator()(std::size_t a, const std::tuple<Ts...>& t) const {
        typedef typename std::tuple_element<0, std::tuple<Ts...>>::type NextT;
        std::size_t b = std::hash<NextT>()(std::get<0>(t));
        return hash_combiner(a, b);
    }
};

ZEN_NAMESPACE_END

template<class...Ts>
struct std::hash<std::tuple<Ts...>> {
    std::size_t operator()(const std::tuple<Ts...>& t) const {
        const std::size_t begin = std::tuple_size<std::tuple<Ts...>>::value-1;
        return zen::tuple_hash_impl<begin, Ts...>()(0, t);
    }
};

