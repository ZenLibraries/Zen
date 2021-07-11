
#include <functional>

namespace std {

  template<
    typename CharT,
    typename Traits,
    typename Allocator
  > struct hash<std::basic_string<CharT, Traits, Allocator>> {

    std::size_t operator()(const std::basic_string<CharT, Traits, Allocator> str) const noexcept {
      std::size_t h = 17;
      for (auto ch: str) {
        h = h * 19 + ch;
      }
      return h;
    }

  };

}


