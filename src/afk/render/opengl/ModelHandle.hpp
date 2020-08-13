#pragma once

#include <vector>

#include "afk/physics/Transform.hpp"
#include "afk/render/Renderer.hpp"

namespace afk {
  namespace render {
    namespace opengl {
      /**
       * Encapsulates a handle to a loaded model.
       */
      struct ModelHandle {
        /** A collection of mesh handles. */
        using Meshes = std::vector<MeshHandle>;

        /** The mesh handles associated with the model. */
        Meshes meshes = {};
      };
    }
  }
};
