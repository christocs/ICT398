#pragma once

// Renderer header
#include "afk/renderer/opengl/Renderer.hpp"
// Handle headers
#include "afk/renderer/opengl/MeshHandle.hpp"
#include "afk/renderer/opengl/ModelHandle.hpp"
#include "afk/renderer/opengl/ShaderHandle.hpp"
#include "afk/renderer/opengl/ShaderProgramHandle.hpp"
#include "afk/renderer/opengl/TextureHandle.hpp"

namespace Afk {
  namespace OpenGl {
    class Renderer;
  }

  using Renderer            = OpenGl::Renderer;
  using ShaderProgramHandle = OpenGl::ShaderProgramHandle;
}
