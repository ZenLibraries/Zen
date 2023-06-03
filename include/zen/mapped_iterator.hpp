
#pragma once

#include <cwchar>
#include <iterator>
#include <type_traits>

namespace zen {

  template<typename IterT, typename F>
  class mapped_iterator {

    IterT iterator;
    F func;

    using traits = std::iterator_traits<IterT>;

  public:

    using iterator_category = traits::iterator_category;

    using value_type = typename std::invoke_result<F, typename IterT::value_type>::type;

    /**
     * Generally speaking, it does not make sense to reference a return type
     * that should be owned, so that is why a reference will always be a plain
     * value_type.
     */
    using reference = value_type;

    using difference_type = traits::difference_type;

    using pointer = traits::pointer;

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

    mapped_iterator& operator++() requires(std::incrementable<IterT>) {
      ++iterator;
      return *this;
    }

    mapped_iterator& operator--() requires(std::bidirectional_iterator<IterT>) {
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
      using difference_type = ZEN_NAMESPACE::mapped_iterator<IterT, F>::difference_type;
      using value_type = ZEN_NAMESPACE::mapped_iterator<IterT, F>::value_type;
      using pointer = ZEN_NAMESPACE::mapped_iterator<IterT, F>::pointer;
      using reference = ZEN_NAMESPACE::mapped_iterator<IterT, F>::reference;
      using iterator_category = ZEN_NAMESPACE::mapped_iterator<IterT, F>::iterator_category;
  };

}

