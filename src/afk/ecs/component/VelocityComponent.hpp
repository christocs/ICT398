#pragma once

#include <glm/glm.hpp>

namespace afk {
  namespace ecs {
    namespace component {
      /**
       * Encapsulates a velocity component.
       */
      struct VelocityComponent {
        /** The entity velocity. */
        glm::vec3 velocity = {};
      };
    }
  }
}
