#pragma once

#include <glad/glad.h>

#include "afk/renderer/Shader.hpp"

namespace Afk {
  namespace OpenGl {
    /**
     * Shader handle - represents a loaded shader
     */
    struct ShaderHandle {
      using Type = Shader::Type;

      GLuint id = {};
      Type type = {};
    };
  }
}
