#pragma once

// renderer header
#include "afk/render/opengl/Renderer.hpp"
// Handle headers
#include "afk/render/opengl/MeshHandle.hpp"
#include "afk/render/opengl/ModelHandle.hpp"
#include "afk/render/opengl/ShaderHandle.hpp"
#include "afk/render/opengl/ShaderProgramHandle.hpp"
#include "afk/render/opengl/TextureHandle.hpp"

namespace afk {
  namespace render {
    namespace opengl {
      class Renderer;
      struct ModelHandle;
    }

    /** The selected renderer. */
    using Renderer            = render::opengl::Renderer;
    using MeshHandle          = render::opengl::MeshHandle;
    using ModelHandle         = render::opengl::ModelHandle;
    using ShaderHandle        = render::opengl::ShaderHandle;
    using ShaderProgramHandle = render::opengl::ShaderProgramHandle;
    using TextureHandle       = render::opengl::TextureHandle;
  }
}
