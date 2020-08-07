#pragma once

#include <filesystem>

namespace afk {
  namespace io {
    /**
     * Returns the absolute path from the specified relative path.
     *
     * @param file_path The relative path.
     * @return The converted absolute path.
     */
    auto get_absolute_path(const std::filesystem::path &file_path) -> std::filesystem::path;
  }
}
