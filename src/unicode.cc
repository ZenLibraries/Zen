
#include <sstream>

#include "zen/unicode.hpp"

namespace zen {

  utf8_stream::utf8_stream(stream<unsigned char>& parent):
    parent(parent) {}

  result<maybe<unicode_char>> utf8_stream::read() {

#define ZEN_GETCHAR(stream, offset, out) \
    auto out ## __result = stream.peek(offset); \
    ZEN_TRY(out ## __result); \
    auto out = *out ## __result

    unicode_char out;

    ZEN_GETCHAR(parent, 1, s0);

    if (!s0.has_value()) {
      return right(std::nullopt);
    }

    if (s0 < 0x80) {

      parent.skip(1);
      out = *s0;

    } else {

      ZEN_GETCHAR(parent, 2, s1);

      if (!s1.has_value()) {
        return left(unicode_unexpected_eof {});
      }

      if ((*s0 & 0xe0) == 0xc0) {

        parent.skip(2);
        out = ((long)(*s0 & 0x1f) <<  6) |
              ((long)(*s1 & 0x3f) <<  0);

      } else {

        ZEN_GETCHAR(parent, 3, s2);

        if (!s2.has_value()) {
          return left(unicode_unexpected_eof {});
        }

        if ((*s0 & 0xf0) == 0xe0) {

          parent.skip(3);
          out = ((long)(*s0 & 0x0f) << 12) |
                ((long)(*s1 & 0x3f) <<  6) |
                ((long)(*s2 & 0x3f) <<  0);

        } else {

          ZEN_GETCHAR(parent, 4, s3);

          if (!s3.has_value()) {
            return left(unicode_unexpected_eof {});
          }

          if ((*s0 & 0xf8) == 0xf0 && (*s0 <= 0xf4)) {

            parent.skip(4);
            out = ((long)(*s0 & 0x07) << 18) |
                  ((long)(*s1 & 0x3f) << 12) |
                  ((long)(*s2 & 0x3f) <<  6) |
                  ((long)(*s3 & 0x3f) <<  0);

          } else {

            parent.skip(1);
            return left(unicode_invalid_byte_sequence {});

          }
        }
      }
    }

    if (out >= 0xd800 && out <= 0xdfff) {
      return left(unicode_invalid_surrogate_half {});
    }

    return right(out);
  }

  unicode_string operator ""_utf8(const char* data, std::size_t sz) {
    iterator_stream<const unsigned char*> chars { (unsigned char*)data, (unsigned char*)data + sz };
    utf8_stream decoder { chars };
    unicode_string out;
    for (;;) {
      auto ch = decoder.get().unwrap();
      if (!ch.has_value()) {
        break;
      }
      out.push_back(*ch);
    }
    return out;
  }

}
