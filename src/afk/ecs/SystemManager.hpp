#pragma once

#include <functional>
#include <vector>

#include "afk/ecs/system/CollisionSystem.hpp"
#include "afk/ecs/system/RenderSystem.hpp"
#include "afk/ecs/system/PhysicsSystem.hpp"

namespace afk {
  namespace ecs {
    /**
     * Manages the systems which operate on entities.
     */
    class SystemManager {
    public:
      /** The system update function. */
      using Update = std::function<void()>;

      /**
       * Registers a system to be called on each display update.
       *
       * @param update The update function to be called.
       */
      auto register_display_update_system(const Update &update) -> void;

      /**
       * Registers a system to be called on each update.
       *
       * @param update The update function to be called.
       */
      auto register_update_system(const Update &update) -> void;

      /**
       * Updates every registered system that should be tied to the render update cycle.
       */
      auto display_update() const -> void;

      /**
       * Updates every registered system that should be tied to the update cycle.
       */
      auto update() const -> void;

    private:
      /** The container of system update functions to run on each display update cycle. */
      std::vector<Update> display_update_systems = {afk::ecs::system::RenderSystem::update};

      /** The container of system update functions to run on each update cycle. */
      std::vector<Update> update_systems = {afk::ecs::system::PhysicsSystem::update};
    };
  }
}
