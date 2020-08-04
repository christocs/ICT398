#pragma once

#include <filesystem>
#include <vector>

#include "afk/render/Shader.hpp"

namespace afk {
  namespace render {
    struct ShaderProgram {
      using ShaderPaths = std::vector<std::filesystem::path>;

      ShaderProgram() = default;
      ShaderProgram(const std::filesystem::path &_file_path);

      ShaderPaths shader_paths        = {};
      std::filesystem::path file_path = {};
    };
  }
}
