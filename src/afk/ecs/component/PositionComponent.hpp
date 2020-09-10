#pragma once

#include <glm/glm.hpp>

namespace afk {
  namespace ecs {
    namespace component {
      /**
       * Encapsulates a position component. Contains an entity's position in
       * 3D space.
       */
      struct PositionComponent {
        /** The entities position. */
        glm::vec3 position = {};
      };
    }
  }
}
