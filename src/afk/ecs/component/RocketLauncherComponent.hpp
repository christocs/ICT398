#pragma once

#include <variant>

#include <glm/glm.hpp>
#include <string>

#include "afk/NumericTypes.hpp"

namespace afk {
  namespace ecs {
    namespace component {
      /**
       * Encapsulates a rocket launcher component which fires a prefab as a projectile
       */
      struct RocketLauncherComponent {
        /** speed to launch the projectile */
        glm::vec3 fire_speed = glm::vec3{1.0f};

        /** Name of prefab to fire */
        std::string prefab_projectile = {};
      };
    }
  }
}
