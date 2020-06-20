#pragma once

#include <vector>

#include "afk/physics/Transform.hpp"
#include "afk/renderer/Renderer.hpp"

namespace Afk {
  namespace OpenGl {
    /**
     * Model handle - represents a loaded model
     */
    struct ModelHandle {
      using Meshes = std::vector<MeshHandle>;

      Meshes meshes = {};
    };
  }
};
