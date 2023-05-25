#ifndef ZEN_BYTESTRING_HPP
#define ZEN_BYTESTRING_HPP

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
  bytestring_view(const BS& data):
    ptr(data.ptr), sz(data.sz) {}

  bool operator==(const char* other) const noexcept {
    return string_equal_helper(ptr, other, sz);
  }

  template<std::size_t N>
  bool operator==(const basic_bytestring<N>& other) const noexcept {
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
  bool operator!=(T other) const {
    return !(*this == other);
  }

  const char& operator[](std::size_t index) const noexcept {
    return ptr[index];
  }


  iterator begin() noexcept {
    return ptr;
  }

  iterator end() noexcept {
    return ptr + sz;
  }

  const_iterator begin() const noexcept {
    return ptr;
  }

  const_iterator end() const noexcept {
    return ptr + sz;
  }

  const_iterator cbegin() const noexcept {
    return ptr;
  }

  const_iterator cend() const noexcept {
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
  std::size_t ptr_sz;
  std::size_t sz;

public:

  using pointer = char*;
  using reference = char&;
  using value_type = char;
  using iterator = char*;
  using const_iterator = const char*;
  using view = bytestring_view;

  basic_bytestring(const char* const other, std::size_t sz): sz(sz) {
    ptr = static_cast<char*>(malloc(sz));
    if (ptr == nullptr) {
      ZEN_PANIC("insufficient memory");
    }
    memcpy(ptr, other, sz);
    ptr_sz = sz;
  }

  basic_bytestring(const char* const other):
    basic_bytestring(other, strlen(other)) {}

  basic_bytestring():
    sz(0) {
      ptr = static_cast<char*>(malloc(N));
      if (ptr == nullptr) {
        ZEN_PANIC("insufficient memory");
      }
      ptr_sz = N;
    }

  basic_bytestring(std::string str): ptr_sz(str.size()), sz(str.size()) {
    ptr = static_cast<char*>(malloc(sz));
    if (ptr == nullptr) {
      ZEN_PANIC("insufficient memory");
    }
    memcpy(ptr, str.c_str(), sz);
  }

  basic_bytestring(const basic_bytestring& other):
    ptr_sz(other.ptr_sz),
    sz(other.sz) {
      ptr = static_cast<char*>(malloc(other.ptr_sz));
      if (ptr == nullptr) {
        ZEN_PANIC("insufficient memory");
      }
      memcpy(ptr, other.ptr, sz);
    }

  basic_bytestring(basic_bytestring&& other):
    ptr(std::move(other.ptr)),
    ptr_sz(std::move(other.ptr_sz)),
    sz(std::move(other.sz)) {
      other.ptr = nullptr;
    }

  iterator begin() noexcept {
    return ptr;
  }

  iterator end() noexcept {
    return ptr + sz;
  }

  const_iterator begin() const noexcept {
    return ptr;
  }

  const_iterator end() const noexcept {
    return ptr + sz;
  }

  const_iterator cbegin() const noexcept {
    return ptr;
  }

  const_iterator cend() const noexcept {
    return ptr + sz;
  }

  bool operator==(const char* other) const noexcept {
    return string_equal_helper(ptr, other, sz);
  }

  template<std::size_t N2>
  bool operator==(const basic_bytestring<N2>& other) const noexcept {
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

  char& operator[](std::size_t index) noexcept {
    return ptr[index];
  }

  const char& operator[](std::size_t index) const noexcept {
    return ptr[index];
  }

  bytestring_view as_view() const noexcept {
    return bytestring_view(*this);
  }

  std::size_t size() const noexcept {
    return sz;
  }

  const char* as_cstr() const {
    return ptr;
  }

  std::string as_std_string() const {
    return std::string(ptr, sz);
  }

  void resize(std::size_t new_sz) {
    ZEN_ASSERT(new_sz <= sz);
    sz = new_sz;
    ptr[new_sz] = '\0';
  }

  ~basic_bytestring() {
    if (ptr != nullptr) {
      free(ptr);
    }
  }

};

template<std::size_t N>
std::ostream& operator<<(std::ostream& out, const basic_bytestring<N>& bs) {
  out << bs.as_cstr();
  return out;
}

using bytestring = basic_bytestring<>;

ZEN_NAMESPACE_END

#endif // #infdef ZEN_BYTESTRING_HPP
