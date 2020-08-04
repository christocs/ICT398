#pragma once

#include <filesystem>
#include <string>

namespace afk {
  namespace render {
    struct Shader {
      enum class Type { Vertex, Fragment };

      std::filesystem::path file_path = {};
      std::string code                = {};
      Type type                       = {};

      Shader() = default;
      Shader(const std::filesystem::path &_file_path);
    };
  }
}
