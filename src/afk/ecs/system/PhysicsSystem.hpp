#pragma once

#include <glm/glm.hpp>

#include "afk/event/Event.hpp"
#include "afk/physics/Transform.hpp"
#include "afk/physics/shape/Box.hpp"
#include "afk/physics/shape/Capsule.hpp"
#include "afk/physics/shape/Sphere.hpp"

namespace afk {
  namespace ecs {
    namespace system {
      /**
       * Handles physics resolution for entities with PhysicsComponent
       */
      struct PhysicsSystem {
        /**
         * Update physics resolution
         */
        static auto update() -> void;

        /**
         * Callback to call when a collision occurs
         *
         * Should only pass in event of type Collision
         *
         * @param event - event information
         */
        static auto collision_resolution_callback(afk::event::Event event) -> void;
      };
    }
  }
}
