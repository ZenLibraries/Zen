#ifndef ZEN_STREAM_HPP
#define ZEN_STREAM_HPP

#include <cstdint>
#include <deque>
#include <iterator>
#include <type_traits>
#include <string>

#include "zen/error.hpp"
#include "zen/maybe.hpp"

namespace zen {

  template<typename T, typename Error = error>
  class stream {
  public:

    virtual result<maybe<T>> get() = 0;
    virtual result<maybe<T>> peek(std::size_t offset = 1) = 0;

    virtual result<void> skip(std::size_t count = 1) {
      for (std::size_t i = 0; i < count; i++) {
        ZEN_TRY_DISCARD(get());
      }
      return right();
    }

  };

  template<typename T>
  class buffered_stream : public stream<T> {

    std::deque<T> buffer;

  public:

    using value_type = T;

    result<maybe<T>> get() override {
      T element;
      if (buffer.empty()) {
        auto result = read();
        ZEN_TRY(result);
        if (!result->has_value()) {
          return right(std::nullopt);
        }
        element = **result;
      } else {
        element = buffer.front();
        buffer.pop_front();
      }
      return right(element);
    }

    result<maybe<T>> peek(std::size_t offset) override {
      while (buffer.size() < offset) {
        auto result = read();
        ZEN_TRY(result);
        if (!result->has_value()) {
          return right(std::nullopt);
        }
        buffer.push_back(**result);
      }
      return right(buffer[offset-1]);
    }

    virtual result<maybe<T>> read() = 0;

  };

  template<typename IterT, typename T = typename std::iterator_traits<IterT>::value_type>
  class iterator_stream : public stream<T> {
  public:

    using value_type = T;

  private:

    IterT current;
    IterT end;

  public:

    iterator_stream(IterT begin, IterT end):
      current(begin), end(end) {}

    result<maybe<value_type>> get() override {
      if (current == end) {
        return right(std::nullopt);
      }
      return right(*(current++));
    }

    result<maybe<value_type>> peek(std::size_t offset = 1) override {
      if (current == end) {
        return right(std::nullopt);
      }
      return right(*current);
    }

  };

  inline iterator_stream<std::string::const_iterator, int> make_stream(const std::string& str) {
    return iterator_stream<std::string::const_iterator, int> {
      str.begin(),
      str.end(),
    };
  }

  inline iterator_stream<std::string_view::const_iterator, int> make_stream(const std::string_view& str) {
    return iterator_stream<std::string_view::const_iterator, int> {
      str.begin(),
      str.end(),
    };
  }

  using char_stream = stream<int>;

}

#endif // of #ifndef ZEN_STREAM_HPP
