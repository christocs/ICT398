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
      enum Difficulty { EASY, NORMAL, HARD };

      DifficultyManager() = default;

      auto init(const Difficulty  &diff) -> void;

      Difficulty get_difficulty();

      auto set_difficulty(const Difficulty &diff) -> void;

    private:
      Difficulty difficulty = Difficulty::NORMAL;
    };
  }
}
