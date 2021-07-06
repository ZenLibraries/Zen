#ifndef ZEN_BYTESTRING_HPP
#define ZEN_BYTESTRING_HPP

#include <string.h>
#include <stdlib.h>

#include <cstdint>
#include <utility>

#include "zen/config.hpp"
#include "zen/algorithm.hpp"
#include "zen/zip_iterator.hpp"

ZEN_NAMESPACE_START

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

  template<std::size_t N>
  bool operator==(const char other[N]) const noexcept {
    auto it = ptr;
    auto end = ptr + sz;
    for (std::size_t i = 0; i < N; ++i) {
      if (it == end || other[i] != it) {
        return false;
      }
    }
    return true;
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

  iterator begin() noexcept {
    return ptr;
  }

  iterator end() noexcept {
    return ptr + sz;
  }

  const_iterator cbegin() const noexcept {
    return ptr;
  }

  const_iterator cend() const noexcept {
    return ptr + sz;
  }

};

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

  const_iterator cbegin() const noexcept {
    return ptr;
  }

  const_iterator cend() const noexcept {
    return ptr + sz;
  }

  template<std::size_t N2>
  bool operator==(const char other[N2]) const noexcept {
    if (sz != N2) {
      return false;
    }
    for (auto i = 0; i < N2; ++i) {
      if (ptr[i] != other[i]) {
        return false;
      }
    }
    return true;
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

  bool operator==(const bytestring_view& other) const noexcept {
    return other == *this;
  }

  bytestring_view as_view() const noexcept {
    return bytestring_view(*this);
  }

  std::size_t size() const noexcept {
    return sz;
  }

  ~basic_bytestring() {
    if (ptr != nullptr) {
      free(ptr);
    }
  }

};

using bytestring = basic_bytestring<>;

ZEN_NAMESPACE_END

#endif // #infdef ZEN_BYTESTRING_HPP
