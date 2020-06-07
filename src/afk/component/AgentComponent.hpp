#pragma once

#include <DetourCrowd.h>

#include "afk/ai/Crowds.hpp"
#include "afk/component/BaseComponent.hpp"
#include "afk/physics/Transform.hpp"
namespace Afk {
  namespace AI {
    class AgentComponent : public BaseComponent {
    public:
      /**
       * \todo: facade dtCrowdAgentParams (once i know that pathfinding works)
       */
      AgentComponent(GameObject e, const glm::vec3 &initial_position, dtCrowdAgentParams &p);
      // AgentComponent(AgentComponent &e)  = delete;
      // AgentComponent(AgentComponent &&e) = delete;
      // auto operator=(AgentComponent &e) -> AgentComponent & = delete;
      // auto operator=(AgentComponent &&e) -> AgentComponent & = delete;
      ~AgentComponent();

      auto target(const glm::vec3 &target_pos) -> void;
      auto update() -> void;

    private:
      float radius;
      Afk::AI::Crowds::AgentID id = -1;
    };
  }
}
