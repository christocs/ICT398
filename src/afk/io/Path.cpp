#include "afk/io/Path.hpp"

#include <filesystem>
#include <string>

#include <cpplocate/cpplocate.h>

#include "afk/debug/Assert.hpp"
#include "afk/io/Unicode.hpp"

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

    auto create_engine_dirs() -> void {
      for (const auto &dir : afk::io::ENGINE_DIRS) {
        auto dir_path   = afk::io::get_resource_path(afk::io::to_cstr(dir));
        auto does_exist = std::filesystem::exists(dir_path);

        if (!does_exist) {
          auto did_create = std::filesystem::create_directory(dir_path);
          afk_assert(did_create, "Failed to create engine directory");
        }
      }
    }
  }
}
