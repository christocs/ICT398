#pragma once

#include <filesystem>
#include <string>

namespace Afk {
  /**
   * Shader (vertex & fragment)
   */
  struct Shader {
    enum class Type { Vertex, Fragment };

    std::filesystem::path file_path = {};
    std::string code                = {};
    Type type                       = {};

    Shader() = default;
    Shader(const std::filesystem::path &_file_path);
  };
}
