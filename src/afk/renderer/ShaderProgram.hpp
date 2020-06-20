#pragma once

#include <filesystem>
#include <vector>

#include "afk/renderer/Shader.hpp"

namespace Afk {
  /**
   * Shader program is a combination of several shaders into one program
   */
  struct ShaderProgram {
    using ShaderPaths = std::vector<std::filesystem::path>;

    ShaderProgram() = default;
    ShaderProgram(const std::filesystem::path &_file_path);

    ShaderPaths shader_paths        = {};
    std::filesystem::path file_path = {};
  };
}
