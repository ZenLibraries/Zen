#ifndef ZEN_CHAR_HPP
#define ZEN_CHAR_HPP

namespace zen {

  inline bool is_digit(char ch) {
    return ch >= 48 && ch <= 57;
  }

  inline bool is_alpha(char ch) {
    return (ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122);
  }

  inline bool is_lalpha(char ch) {
    return ch >= 97 && ch <= 122;
  }

  inline bool is_ualpha(char ch) {
    return ch >= 65 && ch <= 90;
  }

  inline bool is_whitespace(char ch) {
    switch (ch) {
      case ' ':
      case '\n':
      case '\r':
      case '\t':
        return true;
      default:
        return false;
    }
  }

  inline bool is_newline(char ch) {
    return ch == '\n';
  }

  inline int parse_decimal(char ch) {
    return ch - 48;
  }

}

#endif // of #ifndef ZEN_CHAR_HPP
