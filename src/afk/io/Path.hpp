#pragma once

#include <filesystem>

namespace Afk {
  /**
   * Get an absolute path for a path relative to the game root
   */
  auto get_absolute_path(const std::filesystem::path &file_path) -> std::filesystem::path;
}
