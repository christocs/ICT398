#pragma once

#include <glad/glad.h>

namespace afk {
  namespace render {
    namespace opengl {
      /**
       * Encapsulates a handle to a linked OpenGL shader program.
       */
      struct ShaderProgramHandle {
        /** The shader program id. */
        GLuint id = {};
      };
    }
  }
}
