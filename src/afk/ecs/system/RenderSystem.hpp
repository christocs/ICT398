#pragma once

namespace afk {
  namespace ecs {
    namespace system {
      /**
       * Handles rendering entities.
       */
      struct RenderSystem {
        /**
         * Draws all entities with a model and position component.
         */
        static auto update() -> void;
      };
    }
  }
}
