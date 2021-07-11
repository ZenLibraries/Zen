#ifndef ZEN_SEQMAP_HPP
#define ZEN_SEQMAP_HPP

#include <vector>
#include <list>

#include "zen/hash_index.hpp"

ZEN_NAMESPACE_START

template<typename KeyT, typename ValueT>
class seq_map {
public:

  using value_type = std::pair<KeyT, ValueT>;
  using reference = value_type&;
  using size_type = std::size_t;

private:

  struct entry {
    value_type value;
  };

  std::list<value_type> entries;
  hash_index<typename std::list<value_type>::iterator, KeyT> index;

public:

  using iterator = typename std::list<value_type>::iterator;
  using const_iterator = typename std::list<value_type>::const_iterator;

  void emplace(const KeyT& key, const ValueT& value) {
    auto iter = entries.insert(entries.end(), std::make_pair(key, value));
    index.insert(iter);
  }

  size_type size() {
    return entries.size();
  }

  bool empty() const noexcept {
    return entries.empty();
  }

  ValueT& operator[](const KeyT& key) {
    return *index.lookup(key);
  }

  const ValueT& operator[](const KeyT& key) const {
    return *index.lookup(key);
  }

  const_iterator cbegin() const {
    return entries.cbegin();
  }

  const_iterator cend() const {
    return entries.cend();
  }

};

ZEN_NAMESPACE_END

#endif // #ifndef ZEN_SEQMAP_HPP
