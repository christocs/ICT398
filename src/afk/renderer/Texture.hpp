#pragma once

#include <cstddef>
#include <filesystem>
#include <vector>

namespace Afk {
  /**
   * Texture for an object
   */
  struct Texture {
    enum class Type : std::size_t {
      Diffuse = 0,
      Specular,
      Normal,
      Height,
      Count
    };

    Type type                       = Type::Diffuse;
    std::filesystem::path file_path = {};

    Texture() = default;
    Texture(const std::filesystem::path &_file_path);
  };
}
