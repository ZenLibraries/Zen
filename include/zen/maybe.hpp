#ifndef ZEN_OPTIONAL_HPP
#define ZEN_OPTIONAL_HPP

#include <istream>
#include <optional>
#include <type_traits>

namespace zen {

  template<typename T, typename Enabler = void>
  struct derive_maybe {
    using type = std::optional<T>;
  };

  // TODO Add a specialization for unicode_char that uses EOF as the `std::nullopt` marker.

  template<typename T>
  using maybe = typename derive_maybe<T>::type;

}

#endif // #ifndef ZEN_OPTIONAL_HPP
