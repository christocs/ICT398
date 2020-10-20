#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

#include "afk/NumericTypes.hpp"
#include "afk/physics/Transform.hpp"
#include "afk/render/Index.hpp"
#include "afk/render/Mesh.hpp"

namespace afk {
  namespace render {
    struct Vertex {
      /** The position. */
      glm::vec3 position = {};
      glm::vec4 color    = {};
    };

    /**
     * Encapsulates a 3D wireframe mesh.
     */

    struct WireframeMesh {
      /** A collection of vertices. */
      using Vertices = std::vector<Vertex>;
      /** A collection of indicies. */
      using Indices = std::vector<Index>;

      /** The mesh vertices. */
      Vertices vertices = {};
      /** The mesh indices. */
      Indices indices = {};
      /** The mesh transformation. */
      physics::Transform transform = {};
    };
  }
}
