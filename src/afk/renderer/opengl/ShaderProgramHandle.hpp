#pragma once

#include <glad/glad.h>

namespace Afk {
  namespace OpenGl {
    /**
     * Shader program handle - represents a loaded shader
     */
    struct ShaderProgramHandle {
      GLuint id = {};
    };
  }
}
