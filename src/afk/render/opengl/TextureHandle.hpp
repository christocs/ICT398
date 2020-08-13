#pragma once

#include <glad/glad.h>

#include "afk/render/Texture.hpp"

namespace afk {
  namespace render {
    namespace opengl {
      /**
       * Encapsulates a handle to a loaded OpenGL texture.
       */
      struct TextureHandle {
        /** Alias of engine texture types. */
        using Type = Texture::Type;

        /** The texture type. */
        Type type = {};
        /** The texture handle id. */
        GLuint id = {};
        /** The texture width. */
        i32 width = {};
        /** The texture height. */
        i32 height = {};
        /** The number of channels in the texture. */
        i32 channels = 4;
      };
    }
  }
}
