#pragma once

#include <DetourCrowd.h>

#include "afk/ai/Crowds.hpp"
#include "afk/ai/behaviour/BaseBehaviour.hpp"
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

      auto chase(const GameObject &target, float max_dist) -> void;
      auto move_to(const glm::vec3 &target) -> void;

      auto update() -> void;

    private:
      // float radius;
      Afk::AI::Crowds::AgentID id = -1;
      typedef std::shared_ptr<Afk::AI::BaseBehaviour> Behaviour;

      Behaviour current_behaviour = {};
    };
  }
}
