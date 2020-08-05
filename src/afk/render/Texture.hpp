#pragma once

#include <cstddef>
#include <filesystem>
#include <vector>

#include "afk/NumericTypes.hpp"

namespace afk {
  namespace render {
    struct Texture {
      enum class Type : usize { Diffuse = 0, Specular, Normal, Height, Count };

      Type type                       = Type::Diffuse;
      std::filesystem::path file_path = {};

      Texture() = default;
      Texture(const std::filesystem::path &_file_path);
    };
  }
}
