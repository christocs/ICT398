#pragma once

#include <DetourCrowd/Include/DetourCrowd.h>

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
      AgentComponent(GameObject e, Transform *t, dtCrowdAgentParams &p);
      ~AgentComponent();

      auto target(const glm::vec3 &target_pos) -> void;

    private:
      float radius;
      Transform *transform        = nullptr;
      Afk::AI::Crowds::AgentID id = -1;
    };
  }
}
