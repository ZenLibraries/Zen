#ifndef ZEN_FS_FILE_HPP
#define ZEN_FS_FILE_HPP

#include "zen/either.hpp"
#include "zen/bytestring.hpp"
#include "zen/fs/path.hpp"

ZEN_NAMESPACE_START

namespace fs {

either<std::error_code, bytestring> read_file(const path& filename); 

}

ZEN_NAMESPACE_END

#endif
