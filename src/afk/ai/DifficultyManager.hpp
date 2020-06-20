#pragma once

#include <DetourNavMesh.h>
#include <Recast.h>
#include <filesystem>
#include <memory>

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "ChunkyTriMesh.hpp"
#include "afk/physics/Transform.hpp"
#include "afk/renderer/Mesh.hpp"
#include "afk/renderer/Model.hpp"

namespace Afk {
  namespace AI {
    class DifficultyManager {
    public:
      /**
       * Difficulty Levels
       * \todo change to enum class
       */
      enum Difficulty { EASY, NORMAL, HARD };
      /**
       * Default constructor
       */
      DifficultyManager() = default;
      /**
       * Initialize
       */
      auto init(const Difficulty &diff) -> void;
      /**
       * Get the current difficulty level
       */
      Difficulty get_difficulty();
      /**
       * Set the current difficulty level
       */
      auto set_difficulty(const Difficulty &diff) -> void;

    private:
      Difficulty difficulty = Difficulty::NORMAL;
    };
  }
}
