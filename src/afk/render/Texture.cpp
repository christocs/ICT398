#include "afk/render/Texture.hpp"

#include <filesystem>

#include "afk/io/Path.hpp"

using std::filesystem::path;

using afk::render::Texture;

/// @cond DOXYGEN_IGNORE

Texture::Texture(const path &_file_path) {
  this->file_path = _file_path;
}

/// @endcond
