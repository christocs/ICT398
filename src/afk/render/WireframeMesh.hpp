#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

#include "afk/NumericTypes.hpp"
#include "afk/physics/Transform.hpp"
#include "afk/render/Index.hpp"

namespace afk {
  namespace render {
    struct WireframeMesh {
      /**
       * Encapsulates a 3D wireframe mesh.
       */
      struct Vertex {
        using Position = glm::vec3;
        using Color    = glm::vec4;

        /** The vertex position. */
        Position position = {};
        /** The vertex color. */
        Color color = {};
      };

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
