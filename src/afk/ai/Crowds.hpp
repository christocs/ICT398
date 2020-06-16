#pragma once

#include <DetourCrowd.h>
#include <memory>
#include <unordered_set>

#include "afk/ai/NavMeshManager.hpp"

namespace glm {
  struct vec3;
}

namespace Afk {
  namespace AI {
    class Crowds {
    public:
      Crowds()               = default;
      Crowds(Crowds &other)  = delete;
      Crowds(Crowds &&other) = delete;
      auto operator=(Crowds &other) -> Crowds & = delete;
      auto operator=(Crowds &&other) -> Crowds & = delete;

      auto update(float dt_seconds) -> void;
      auto init(NavMeshManager::nav_mesh_ptr nav_mesh) -> void;
      auto current_crowd() -> dtCrowd &;

      auto nearest_pos(glm::vec3 req, float search_dist = 10.f) -> glm::vec3;

      typedef int AgentID;

    private:
      typedef std::unique_ptr<dtCrowd, decltype(&dtFreeCrowd)> crowd_ptr;
      crowd_ptr crowd = crowd_ptr{dtAllocCrowd(), &dtFreeCrowd};
    };
  }
}
