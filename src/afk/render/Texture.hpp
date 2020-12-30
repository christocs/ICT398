#pragma once

#include <cstddef>
#include <filesystem>
#include <vector>

#include "afk/NumericTypes.hpp"

namespace afk {
  namespace render {
    /**
     * Encapsulates a texture properties, but no image data.
     */
    struct Texture {
      /** The texture type. */
      enum class Type : usize { Diffuse = 0, Specular, Normal, Height, Count };

      /** The texture type. */
      Type type = Type::Diffuse;
      /** The path to the texture file. */
      std::filesystem::path file_path = {};

      Texture() = default;

      /**
       * Constructs and loads a texture from the specified file path.
       *
       * @param _file_path The path to construct the texture from.
       */
      Texture(const std::filesystem::path &_file_path);
    };
  }
}
