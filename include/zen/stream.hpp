#ifndef ZEN_STREAM_HPP
#define ZEN_STREAM_HPP

#include <cstdint>
#include <deque>
#include <type_traits>
#include <string>

namespace zen {

  template<typename T>
  class stream {
  public:
    virtual T get() = 0;
    virtual T peek(std::size_t offset = 1) = 0;
  };

  template<typename T>
  class buffered_stream : public stream<T> {

    std::deque<T> buffer;

  public:

    using value_type = T;

    T get() override {
      T element;
      if (buffer.empty()) {
        element = read();
      } else {
        element = buffer.front();
        buffer.pop_front();
      }
      return element;
    }

    T peek(std::size_t offset) override {
      while (buffer.size() < offset) {
        buffer.push_back(read());
      }
      return buffer[offset-1];
    }

    virtual T read() = 0;

  };

  template<typename IterT, typename T = typename std::iterator_traits<IterT>::value_type>
  class iterator_stream : public stream<T> {
  public:

    using value_type = T;

  private:

    value_type sentry;
    IterT current;
    IterT end;

  public:

    iterator_stream(IterT begin, IterT end, value_type sentry):
      current(begin), end(end), sentry(sentry) {}

    value_type get() override {
      return current == end ? sentry : *(current++);
    }

    value_type peek(std::size_t offset = 1) override {
      return current == end ? sentry : *current;
    }

  };

  inline iterator_stream<std::string::const_iterator, int> make_stream(const std::string& str) {
    return iterator_stream<std::string::const_iterator, int> {
      str.begin(),
      str.end(),
      EOF
    };
  }

  inline iterator_stream<std::string_view::const_iterator, int> make_stream(const std::string_view& str) {
    return iterator_stream<std::string_view::const_iterator, int> {
      str.begin(),
      str.end(),
      EOF
    };
  }

  using char_stream = stream<int>;

}

#endif // of #ifndef ZEN_STREAM_HPP
