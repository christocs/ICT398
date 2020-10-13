#pragma once

#include "afk/ecs/Registry.hpp"
#include "afk/ecs/SystemManager.hpp"

namespace afk {
  namespace ecs {
    /**
     * Encapsulates the entity component system (ECS).
     */
    class Ecs {
    public:
      /** The entity registry. */
      afk::ecs::Registry registry = {};
      /** The systems that operate on the entities. */
      afk::ecs::SystemManager system_manager = {};

      /**
       * Updates every registered system.
       */
      auto update() -> void;

      /**
       * Initializes the ECS subsystem.
       */
      auto initialize() -> void;
    };
  }
}
