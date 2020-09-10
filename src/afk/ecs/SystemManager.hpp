#pragma once

#include <functional>
#include <vector>

#include "afk/ecs/system/RenderSystem.hpp"

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
       * Registers a system.
       *
       * @param update The update function to be called.
       */
      auto register_system(Update update) -> void;

      /**
       * Updates every registered system.
       */
      auto update() const -> void;

    private:
      /** The container of system update functions. */
      std::vector<Update> systems = {afk::ecs::system::RenderSystem::update};
    };
  }
}
