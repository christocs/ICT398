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
      class renderer;
    }

    using renderer            = render::opengl::renderer;
    using ShaderProgramHandle = render::opengl::ShaderProgramHandle;
  }
}
