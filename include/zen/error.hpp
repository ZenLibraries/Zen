#ifndef ZEN_ERROR_HPP
#define ZEN_ERROR_HPP

#include <variant>

#include "zen/either.hpp"

namespace zen {

  struct unicode_invalid_surrogate_half {
  };

  struct unicode_invalid_byte_sequence {
  };

  struct unicode_unexpected_eof {
  };

  struct reached_end_of_stream {
  };

  using error = std::variant<
    unicode_unexpected_eof,
    unicode_invalid_surrogate_half,
    unicode_invalid_byte_sequence,
    reached_end_of_stream
  >;

  template<typename T>
  using result = either<error, T>;

}

#endif // #ifndef ZEN_ERROR_HPP

