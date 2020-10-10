#pragma once

#include "glm/vec3.hpp"

namespace afk {
  namespace physics {
    namespace shape {
      /**
       * Capsule shape
       */
      struct Capsule {
        float radius = 1.0f;
        float height = 2.0f;
      };
    }
  }
}
