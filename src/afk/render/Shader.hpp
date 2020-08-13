#pragma once

#include <filesystem>
#include <string>

namespace afk {
  namespace render {
    /**
     * Encapsulates a shader.
     */
    struct Shader {
      /** Represents a shader type. */
      enum class Type { Vertex, Fragment };

      /** The shader file path. */
      std::filesystem::path file_path = {};
      /** The shader code. */
      std::string code = {};
      /** The shader type. */
      Type type = {};

      Shader() = default;

      /**
       * Constructs and loads a shader from the specified file path.
       *
       * @param _file_path The shader file path.
       */
      Shader(const std::filesystem::path &_file_path);
    };
  }
}
