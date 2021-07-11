#ifndef ZEN_HASHINDEX_HPP
#define ZEN_HASHINDEX_HPP

#include <type_traits>
#include <vector>

#include "zen/iterator_adaptor.hpp"
#include "zen/hash.hpp"

ZEN_NAMESPACE_START

template<typename T, typename KeyT>
using hash_bucket = std::vector<T>;

template<
  typename T,
  typename KeyT,
  typename ReferenceT,
  typename BucketT
>
class hash_index_iterator 
  : iterator_adaptor<
      hash_index_iterator <
        T,
        KeyT,
        ReferenceT,
        BucketT
      >,
      T,
      ReferenceT
    > {

  BucketT& buckets;
  std::size_t bucket_index;
  std::size_t element_index;

public:

  using value_type = T;

  inline hash_index_iterator(
    BucketT& buckets,
    std::size_t bucket_index,
    std::size_t element_index
  ): buckets(buckets),
     bucket_index(bucket_index),
     element_index(element_index) {}

  value_type dereference() {
    return buckets[bucket_index][element_index];
  }

  void increment() {
    for (;;) {
      if (bucket_index >= buckets.size()) {
        break;
      }
      if (element_index >= buckets[bucket_index].size()) {
        ++bucket_index;
      } else {
        ++element_index;
      }
      if (element_index < buckets[bucket_index].size()) {
        break;
      }
    }
  }

  void decrement() {
    for (;;) {
      if (element_index == 0) {
        --bucket_index;
        element_index = buckets[bucket_index].size();
      } else {
        --element_index;
      }
      if (element_index < buckets[bucket_index].size()) {
        break;
      }
    }
  }

  bool operator==(const hash_index_iterator& other) const {
    return bucket_index == other.bucket_index
        && element_index == other.element_index;
  }

};

template<
  typename T,
  typename KeyT,
  typename ReferenceT,
  typename BucketT
>
class const_hash_index_iterator : hash_index_iterator<const T, KeyT, const ReferenceT, const BucketT> {

};

template<typename T, typename KeyT = T>
class hash_index {

  using bucket = hash_bucket<T, KeyT>;

  std::hash<KeyT> hasher;

  std::vector<bucket> buckets;

  const KeyT& get_key(const T& element) {
    // FIXME This needs to be generalized
    return element->first;
  }

public:

  hash_index() {
    buckets.reserve(256);
    for (std::size_t i = 0; i < 256; ++i) {
      buckets.push_back(bucket {});
    }
  }

  using value_type = T;
  using reference = T&;

  using iterator = hash_index_iterator< T, KeyT, reference, bucket >;
  using const_iterator = const_hash_index_iterator< T, KeyT, reference, bucket >;

  void insert(T element) {
    const auto& key = get_key(element);
    const auto h = hasher(key);
    auto& bucket = buckets[h % buckets.size()];
    bucket.push_back(element);
  }

  iterator lookup(const KeyT& key) {
    const auto h = hasher(key);
    const auto bucket_index = h % buckets.size();
    auto& bucket = buckets[bucket_index];
    std::size_t i = 0;
    const auto end = bucket.end();
    for (auto it = bucket.begin(); it != end; ++it, ++i) {
      if (get_key(*it) == key) {
        return hash_index_iterator(bucket, bucket_index, i);
      }
    }
    return end();
  }

  const_iterator lookup(const KeyT& key) const {
    const auto h = hasher(key);
    const auto bucket_index = h % buckets.size();
    auto& bucket = buckets[bucket_index];
    std::size_t i = 0;
    const auto end = bucket.end();
    for (auto it = bucket.begin(); it != end; ++it, ++i) {
      if (get_key(*it) == key) {
        return const_hash_index_iterator(bucket, bucket_index, i);
      }
    }
    return cend();
  }

  iterator begin() noexcept {
    return iterator(buckets, 0, 0);
  }

  iterator end() noexcept {
    return iterator(buckets, buckets.size(), 0);
  }

  const_iterator begin() const noexcept {
    return cbegin();
  }

  const_iterator end() const noexcept {
    return cend();
  }

  const_iterator cbegin() const noexcept {
    return const_iterator(buckets, 0, 0);
  }

  const_iterator cend() const noexcept {
    return const_iterator(buckets, buckets.size(), 0);
  }

};

ZEN_NAMESPACE_END

#endif // of #ifndef ZEN_HASHINDEX_HPP
