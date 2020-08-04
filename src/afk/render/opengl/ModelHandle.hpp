#pragma once

#include <vector>

#include "afk/physics/Transform.hpp"
#include "afk/render/Renderer.hpp"

namespace afk {
  namespace render {
    namespace opengl {
      struct ModelHandle {
        using Meshes = std::vector<MeshHandle>;

        Meshes meshes = {};
      };
    }
  }
};
