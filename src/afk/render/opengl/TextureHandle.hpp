#pragma once

#include <glad/glad.h>

#include "afk/render/Texture.hpp"

namespace afk {
  namespace render {
    namespace opengl {
      struct TextureHandle {
        using Type = Texture::Type;

        Type type    = {};
        GLuint id    = {};
        i32 width    = {};
        i32 height   = {};
        i32 channels = 4;
      };
    }
  }
}
