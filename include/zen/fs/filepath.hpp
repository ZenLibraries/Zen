#ifndef ZEN_FILEPATH_HPP
#define ZEN_FILEPATH_HPP

#include "zen/bytestring.hpp"
#include "zen/either.hpp"

namespace zen {

  namespace fs {

    enum class error {
      generic_error,
      out_of_memory,
      file_does_not_exist,
      bad_file_descriptor,
      access_denied,
      symlink_loop,
      bad_address,
      not_a_directory,
      file_overflow,
    };

    template<typename T>
    using result = either<error, T>;

    class filepath_iterator {
    };

    class const_filepath_iterator {
    };

    class filepath {

      friend std::ostream& operator<<(std::ostream& out, const filepath& fp);

      bytestring contents;

    public:

      using iterator = filepath_iterator;
      using const_iterator = const_filepath_iterator;

      inline filepath(const char* contents):
        contents(contents) {}

      inline filepath(bytestring contents):
        contents(contents) {}

      void normalize();

      std::string as_std_string() const {
        return contents.as_std_string();
      }

      result<bool> exists();

      iterator begin();
      iterator end();

      const_iterator cbegin();
      const_iterator cend();

      bool operator==(const filepath& other) const noexcept {
        return contents == other.contents;
      }

      bool operator==(const char* other) const noexcept {
        return contents == other;
      }

    };

    std::ostream& operator<<(std::ostream& out, const filepath& fp);

  }

}

#endif // of #ifndef ZEN_FILEPATH_HPP
