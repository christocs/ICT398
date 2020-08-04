#pragma once

#include <glad/glad.h>

#include "afk/render/Shader.hpp"

namespace afk {
  namespace render {
    namespace opengl {
      struct ShaderHandle {
        using Type = Shader::Type;

        GLuint id = {};
        Type type = {};
      };
    }
  }
}
