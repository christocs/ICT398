#pragma once

#include <glad/glad.h>

#include "afk/render/Shader.hpp"

namespace afk {
  namespace render {
    namespace opengl {
      /**
       * Encapsulates a handle to a loaded OpenGL shader.
       */
      struct ShaderHandle {
        /** The shader type. */
        using Type = Shader::Type;

        /** The shader id. */
        GLuint id = {};
        /** The shader type. */
        Type type = {};
      };
    }
  }
}
