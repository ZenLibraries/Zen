#ifndef ZEN_BYTESTRING_HPP
#define ZEN_BYTESTRING_HPP

#include <cstdlib>
#include <string.h>
#include <stdlib.h>

#include <cstdint>
#include <utility>

#include "zen/config.hpp"
#include "zen/algorithm.hpp"
#include "zen/zip_iterator.hpp"
#include "zen/range.hpp"

ZEN_NAMESPACE_START

inline bool string_equal_helper(const char* a, const char* b, std::size_t a_sz) {
  for (std::size_t i = 0;; ++i) {
    if (i == a_sz) {
      return *b == '\0';
    }
    auto ch_a = *a;
    auto ch_b = *b;
    if (ch_b == '\0' || ch_a != ch_b) {
      return false;
    }
    ++a;
    ++b;
  }
  return true;
}

template<std::size_t N = 1024>
class basic_bytestring;

class bytestring_view {
public:

  using reference = const char&;
  using value_type = const char;
  using iterator = const char*;
  using const_iterator = const char*;

  const char* ptr;
  const std::size_t sz;

  template<typename BS>
  bytestring_view(const BS& data) ZEN_NOEXCEPT:
    ptr(data.ptr), sz(data.sz) {}

  bool operator==(const char* other) const ZEN_NOEXCEPT {
    return string_equal_helper(ptr, other, sz);
  }

  template<std::size_t N>
  bool operator==(const basic_bytestring<N>& other) const ZEN_NOEXCEPT {
    if (sz != other.sz) {
      return false;
    }
    for (auto [c1, c2]: zip(*this, other)) {
      if (c1 != c2) {
        return false;
      }
    }
    return true;
  }

  // FIXME Implement this properly
  template<typename T>
  bool operator!=(const T& other) const ZEN_NOEXCEPT {
    return !(*this == other);
  }

  const char& operator[](std::size_t index) const ZEN_NOEXCEPT {
    return ptr[index];
  }

  iterator begin() ZEN_NOEXCEPT {
    return ptr;
  }

  iterator end() ZEN_NOEXCEPT {
    return ptr + sz;
  }

  const_iterator begin() const ZEN_NOEXCEPT {
    return ptr;
  }

  const_iterator end() const ZEN_NOEXCEPT {
    return ptr + sz;
  }

  const_iterator cbegin() const ZEN_NOEXCEPT {
    return ptr;
  }

  const_iterator cend() const ZEN_NOEXCEPT {
    return ptr + sz;
  }

};

inline std::ostream& operator<<(std::ostream& out, const bytestring_view& bs) {
  for (auto ch: bs) {
    out << ch;
  }
  return out;
}

template<std::size_t N>
class basic_bytestring {

  friend class bytestring_view;

  char* ptr;
  std::size_t max_sz;
  std::size_t sz;

public:

  using pointer = char*;
  using reference = char&;
  using value_type = char;
  using iterator = char*;
  using const_iterator = const char*;
  using view = bytestring_view;

  basic_bytestring(std::size_t max_sz) ZEN_NOEXCEPT:
    sz(0) {
      ptr = static_cast<char*>(malloc(max_sz));
      if (ptr == nullptr) {
        ZEN_PANIC("insufficient memory");
      }
      max_sz = max_sz;
    }

  basic_bytestring() ZEN_NOEXCEPT:
    basic_bytestring(N) {}

  basic_bytestring(const char* const other, std::size_t other_sz) ZEN_NOEXCEPT:
    basic_bytestring(other_sz) {
      sz = other_sz;
      memcpy(ptr, other, sz);
    }

  basic_bytestring(const char* const other) ZEN_NOEXCEPT:
    basic_bytestring(other, strlen(other)) {}

  basic_bytestring(const std::string& str) ZEN_NOEXCEPT:
    basic_bytestring(str.c_str(), str.size()) {}

  basic_bytestring(const basic_bytestring& other) ZEN_NOEXCEPT:
    basic_bytestring(other.ptr, other.sz) {}

  basic_bytestring(basic_bytestring&& other) ZEN_NOEXCEPT:
    ptr(std::move(other.ptr)),
    max_sz(std::move(other.max_sz)),
    sz(std::move(other.sz)) {
      other.ptr = nullptr;
    }

  iterator begin() ZEN_NOEXCEPT {
    return ptr;
  }

  iterator end() ZEN_NOEXCEPT {
    return ptr + sz;
  }

  const_iterator begin() const ZEN_NOEXCEPT {
    return ptr;
  }

  const_iterator end() const ZEN_NOEXCEPT {
    return ptr + sz;
  }

  const_iterator cbegin() const ZEN_NOEXCEPT {
    return ptr;
  }

  const_iterator cend() const ZEN_NOEXCEPT {
    return ptr + sz;
  }

  bool operator==(const char* other) const ZEN_NOEXCEPT {
    return string_equal_helper(ptr, other, sz);
  }

  template<std::size_t N2>
  bool operator==(const basic_bytestring<N2>& other) const ZEN_NOEXCEPT {
    if (sz != other.sz) {
      return false;
    }
    for (std::size_t i = 0; i < sz; ++i) {
      if (ptr[i] != other.ptr[i]) {
        return false;
      }
    }
    return true;
  }

  char& operator[](std::size_t index) ZEN_NOEXCEPT {
    return ptr[index];
  }

  const char& operator[](std::size_t index) const ZEN_NOEXCEPT {
    return ptr[index];
  }

  bytestring_view as_view() const ZEN_NOEXCEPT {
    return bytestring_view(*this);
  }

  std::size_t capacity() const ZEN_NOEXCEPT {
    return max_sz;
  }

  std::size_t size() const ZEN_NOEXCEPT {
    return sz;
  }

  const char* c_str() const ZEN_NOEXCEPT {
    return ptr;
  }

  char* data() const ZEN_NOEXCEPT {
    return ptr;
  }

  std::string to_std_string() const ZEN_NOEXCEPT {
    return std::string(ptr, sz);
  }

  void resize(std::size_t new_sz) ZEN_NOEXCEPT{
    ZEN_ASSERT(new_sz <= sz);
    sz = new_sz;
    ptr[new_sz] = '\0';
  }

  ~basic_bytestring() ZEN_NOEXCEPT {
    if (ptr != nullptr) {
      free(ptr);
    }
  }

};

template<std::size_t N>
std::ostream& operator<<(std::ostream& out, const basic_bytestring<N>& bs) ZEN_NOEXCEPT {
  out << bs.c_str();
  return out;
}

using bytestring = basic_bytestring<>;

ZEN_NAMESPACE_END

namespace std {

  template<std::size_t N>
  struct hash<zen::basic_bytestring<N>> {

    std::size_t operator()(const zen::basic_bytestring<N> str) const noexcept {
      std::size_t h = 17;
      for (auto ch: str) {
        h = h * 19 + ch;
      }
      return h;
    }

  };

}


#endif // #infdef ZEN_BYTESTRING_HPP
