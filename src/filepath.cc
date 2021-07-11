
#include "sys/stat.h"

#include "zen/fs/filepath.hpp"

namespace zen {

  namespace fs {

    error map_errno_to_error(int code) {
      switch (code) {
        case EACCES:
          return error::access_denied;
        case EBADF:
          return error::bad_file_descriptor;
        case EOVERFLOW:
          return error::file_overflow;
        case ENOMEM:
          return error::out_of_memory;
        case ENOENT:
          return error::file_does_not_exist;
        case ENOTDIR:
          return error::not_a_directory;
        default:
          return error::generic_error;
      }
    }

    void filepath::normalize() {
      std::size_t k = 0;
      for (std::size_t i = 0; i < contents.size(); ++i) {
        auto c0 = contents[i];
        if (i+1 < contents.size() - 1) {
          auto c1 = contents[i+1];
          auto prev_char_was_dot = i > 0 && contents[i-1] == '.';
          if (!prev_char_was_dot && c0 == '.' && c1 == '/') {
            i += 1;
            continue;
          }
        }
        contents[k++] = contents[i];
      }
      contents.resize(k);
    }

    result<bool> filepath::exists() {
      struct stat s;
      if (stat(contents.as_cstr(), &s)) {
        switch (errno) {
          case ENOENT:
            return right(false);
          default:
            return left(map_errno_to_error(errno));
        }
      }
      return right(true);
    }

    std::ostream& operator<<(std::ostream& out, const filepath& fp) {
      out << fp.contents.as_cstr();
      return out;
    }

  }

}

