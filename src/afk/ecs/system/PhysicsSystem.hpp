#pragma once

#include "afk/event/Event.hpp"

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

        static auto collision_resolution_callback(afk::event::Event event) -> void;
      };
    }
  }
}
