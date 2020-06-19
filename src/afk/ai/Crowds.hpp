#pragma once

#include <DetourCrowd.h>
#include <memory>
#include <optional>
#include <unordered_set>

#include <glm/glm.hpp>

#include "afk/ai/NavMeshManager.hpp"

namespace Afk {
  namespace AI {
    /**
     * AI Agent Crowd Management
     */
    class Crowds {
    public:
      typedef int AgentID;

      Crowds()               = default;
      Crowds(Crowds &other)  = delete;
      Crowds(Crowds &&other) = delete;
      auto operator=(Crowds &other) -> Crowds & = delete;
      auto operator=(Crowds &&other) -> Crowds & = delete;
      /**
       * Update with delta time in seconds
       */
      auto update(float dt_seconds) -> void;
      /**
       * Initialize nav mesh
       */
      auto init(NavMeshManager::nav_mesh_ptr nav_mesh) -> void;
      /**
       * Get current crowd
       * \todo façade
       */
      auto current_crowd() -> dtCrowd &;
      /**
       * Find nearest position on the nav mesh with dist search_dist
       * \param req requested position
       * \param search_dist searches square in each search_dist direction
       * \return optional containing the found position, or nothing
       */
      auto nearest_pos(glm::vec3 req, float search_dist = 10.f)
          -> std::optional<glm::vec3>;
      /**
       * Move an agent
       */
      auto request_move(AgentID id, glm::vec3 pos, float search_dist = 10.f) -> void;

    private:
      typedef std::unique_ptr<dtCrowd, decltype(&dtFreeCrowd)> crowd_ptr;
      crowd_ptr crowd = crowd_ptr{dtAllocCrowd(), &dtFreeCrowd};
    };
  }
}
