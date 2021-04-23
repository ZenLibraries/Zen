#ifndef ZEN_ASSOC_LIST_HPP
#define ZEN_ASSOC_LIST_HPP

#include <iostream>
#include <vector>
#include <string>

namespace zen {

  template<typename K, typename V>
  class assoc_list {

    std::vector<std::pair<K, V>> elements;

    template<typename T>
    struct needle_type_helper {
      using type = T;
    };

    template<typename CharT, typename Traits, typename Alloc>
    struct needle_type_helper<std::basic_string<CharT, Traits, Alloc>> {
      using type = std::basic_string_view<CharT, Traits>;
    };

    using needle_type = typename needle_type_helper<K>::type;

  public:

    using value_type = std::pair<K, V>;
    using iterator = typename std::vector<std::pair<K, V>>::iterator;
    using const_iterator = typename std::vector<std::pair<K, V>>::const_iterator;
    using size_type = typename std::vector<std::pair<K, V>>::size_type;

    assoc_list() {}

    assoc_list(std::initializer_list<std::pair<K, V>> elements):
      elements(elements) {}

    assoc_list(const assoc_list& other):
      elements(other.elements) {}

    assoc_list(assoc_list&& other):
      elements(std::move(other.elements)) {}

    bool empty() const {
      return elements.empty();
    }

    size_type size() const {
      return elements.size();
    }

    V& operator[](const needle_type& key) {
      for (auto& [k, v]: elements) {
        if (k == key) {
          return v;
        }
      }
    }

    template<typename ...ForwardArg>
    iterator push_back(ForwardArg&& ...args) {
      elements.push_back(std::make_pair(std::forward<ForwardArg>(args)...));
      return elements.end();
    }

    iterator find(const needle_type& key) {
      auto end = elements.end();
      for (auto iter = elements.begin(); iter != end; ++iter) {
        if (iter->first == key) {
          return iter;
        }
      }
      return end;
    }

    bool contains(const needle_type& key) const {
      auto end = elements.end();
      for (auto iter = elements.begin(); iter != end; ++iter) {
        if (iter->first == key) {
          return true;
        }
      }
      return false;
    }

    const_iterator cfind(const needle_type& key) const { 
      auto end = elements.end();
      for (auto iter = elements.begin(); iter != end; ++iter) {
        if (iter->first == key) {
          return iter;
        }
      }
      return end;
    }

    iterator begin() {
      return elements.begin();
    }

    iterator end() {
      return elements.end();
    }

    const_iterator begin() const {
      return elements.cbegin();
    }

    const_iterator end() const {
      return elements.cend();
    }

  };

}

#endif // of #ifndef ZEN_ASSOC_LIST_HPP
