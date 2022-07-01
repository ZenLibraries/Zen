#ifndef ZEN_UNICODE_HPP
#define ZEN_UNICODE_HPP

#include <cstdint>
#include <istream>

#include "zen/error.hpp"
#include "zen/json.hpp"
#include "zen/stream.hpp"

namespace zen {

  using unicode_char = std::uint32_t;
  using unicode_string = std::basic_string<unicode_char>;

  static constexpr const unicode_char eof = 0xFFFF;

  class utf8_stream : public buffered_stream<unicode_char> {

    stream<unsigned char>& parent;

  public:

    utf8_stream(stream<unsigned char>& parent);

    result<maybe<unicode_char>> read() override;

  };

  unicode_string operator ""_utf8(const char* data, std::size_t sz);

}

#endif // #ifndef ZEN_UNICODE_HPP
