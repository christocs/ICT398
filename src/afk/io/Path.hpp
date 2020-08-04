#pragma once

#include <filesystem>

namespace afk {
  namespace io {
    auto get_absolute_path(const std::filesystem::path &file_path) -> std::filesystem::path;
  }
}
