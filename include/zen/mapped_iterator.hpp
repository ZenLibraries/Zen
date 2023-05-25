
#pragma once

#include <cwchar>
#include <iterator>
#include <type_traits>

#include "zen/iterator_adaptor.hpp"

namespace zen {

  template<typename IterT, typename F>
  class mapped_iterator {

    IterT iterator;
    F func;

  public:

    using iterator_category = std::random_access_iterator_tag;

    // NOTE Make sure to adjust the template argument when changing this type
    using value_type = typename std::invoke_result<F, typename IterT::value_type>::type;

    /**
     * Generally speaking, it does not make sense to reference a return type, so
     * that is why a reference will always be a plain value_type.
     */
    using reference = value_type;

    using difference_type = IterT::difference_type;

    using pointer = IterT::pointer;

    mapped_iterator(IterT iterator, F func):
      iterator(iterator), func(func) {}

    // TODO make this work with arbitrary F
    bool operator==(const mapped_iterator& other) const {
      return iterator == other.iterator;
    }

    // TODO make this work with arbitrary F
    bool operator!=(const mapped_iterator& other) const {
      return iterator != other.iterator;
    }

    IterT& operator++() requires(std::incrementable<IterT>) {
      ++iterator;
      return *this;
    }

    IterT& operator--() requires(std::bidirectional_iterator<IterT>) {
      --iterator;
      return *this;
    }

    mapped_iterator operator++(int) requires(std::incrementable<IterT>) {
      return mapped_iterator { iterator++, func };
    }

    mapped_iterator operator--(int) requires(std::bidirectional_iterator<IterT>) {
      return mapped_iterator { iterator--, func };
    }

    IterT& operator+(std::ptrdiff_t offset) requires(std::random_access_iterator<IterT>) {
      iterator += offset;
      return *this;
    }

    IterT& operator-(std::ptrdiff_t offset) requires(std::random_access_iterator<IterT>) {
      iterator -= offset;
      return *this;
    }

    reference operator*() requires(std::indirectly_readable<IterT>) {
      return func(*iterator);
    }

    IterT& operator=(value_type&& value) requires(std::indirectly_writable<IterT, value_type>) {
      iterator = value;
      return *this;
    }

  };

  template<typename IterT, typename F>
  auto map(IterT iterator, F func) {
    return mapped_iterator { iterator, func };
  }

}

namespace std {

  template <typename IterT, typename F>
  struct iterator_traits<zen::mapped_iterator<IterT, F>>
  {
      using difference_type = zen::mapped_iterator<IterT, F>::difference_type;
      using value_type = zen::mapped_iterator<IterT, F>::value_type;
      using pointer = zen::mapped_iterator<IterT, F>::pointer;
      using reference = zen::mapped_iterator<IterT, F>::reference;
      using iterator_category = random_access_iterator_tag;
  };
}

