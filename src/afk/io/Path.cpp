#include "afk/io/Path.hpp"

#include <filesystem>
#include <string>

#include <cpplocate/cpplocate.h>

using std::string;
using std::filesystem::path;

/**
 * Normalizes a path to handle platform specific nonsense. At the moment this
 * function only converts backslashes to forward slashes.
 *
 * @param raw_path The raw path.
 * @return The normalized path.
 */
static auto normalise_path(const string &raw_path) -> path {
  auto tmp = raw_path;

#ifdef WIN32
  std::replace(tmp.begin(), tmp.end(), '\\', '/');
#endif

  return path{tmp};
}

namespace afk {
  namespace io {
    auto get_resource_path(const path &file_path) -> path {
      static auto root_dir = path{normalise_path(cpplocate::getModulePath())};

      return root_dir / file_path;
    }
  }
}
