#pragma once

#include <glad/glad.h>

#include "afk/renderer/Texture.hpp"

namespace Afk {
  namespace OpenGl {
    /**
     * Texture handle - represents a loaded texture
     */
    struct TextureHandle {
      using Type = Texture::Type;

      Type type    = {};
      GLuint id    = {};
      int width    = {};
      int height   = {};
      int channels = 4;
    };
  }
}
