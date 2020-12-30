#pragma once

#include <filesystem>
#include <vector>

#include "afk/render/Shader.hpp"

namespace afk {
  namespace render {
    /**
     * Encapsulates a shader program. A shader program consists of multiple
     * shaders, which are linked together in order to be used.
     */
    struct ShaderProgram {
      /** Container of shader paths. */
      using ShaderPaths = std::vector<std::filesystem::path>;

      ShaderProgram() = default;

      /**
       * Constructs and loads a shader program from the specified file path.
       *
       * @param _file_path The shader program file path.
       */
      ShaderProgram(const std::filesystem::path &_file_path);

      /** A list of paths to the shader files. */
      ShaderPaths shader_paths = {};
      /** The path of the shader program file. */
      std::filesystem::path file_path = {};
    };
  }
}
