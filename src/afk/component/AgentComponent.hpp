#pragma once

#include <DetourCrowd.h>

#include "afk/ai/Crowds.hpp"
#include "afk/ai/behaviour/BaseBehaviour.hpp"
#include "afk/ai/behaviour/Path.hpp"
#include "afk/component/BaseComponent.hpp"
#include "afk/physics/Transform.hpp"

namespace Afk {
  namespace AI {
    class AgentComponent : public BaseComponent {
    public:
      /**
       * \todo: façade dtCrowdAgentParams
       */
      AgentComponent(GameObject e, const glm::vec3 &initial_position, dtCrowdAgentParams &p);
      AgentComponent(AgentComponent &e) = delete;
      /**
       * move constructor requried for the ECS
       */
      AgentComponent(AgentComponent &&e); // = delete;
      auto operator=(AgentComponent &e) -> AgentComponent & = delete;
      /**
       * move assignment required for the ECS
       */
      auto operator=(AgentComponent &&e) -> AgentComponent &; // = delete;
      ~AgentComponent();

      /**
       * \sa Afk::AI::Chase
       */
      auto chase(const GameObject &target, float max_dist) -> void;
      /**
       * \sa Afk::AI::Flee
       */
      auto flee(const GameObject &target, float desired_dist) -> void;
      /**
       * \sa Afk::AI::MoveTo
       */
      auto move_to(const glm::vec3 &target) -> void;
      /**
       * \sa Afk::AI::PathFollow
       */
      auto path(const Afk::AI::Path &path, float min_dist) -> void;
      /**
       * \sa Afk::AI::Wander
       */
      auto wander(const glm::vec3 &target, float radius) -> void;
      /**
       * Update entity data based on agent data
       */
      auto update() -> void;
      /**
       * Agent's ID
       */
      auto get_id() const -> Afk::AI::Crowds::AgentID;

    private:
      Afk::AI::Crowds::AgentID id = -1;
      // float radius;
      typedef std::shared_ptr<Afk::AI::BaseBehaviour> Behaviour;

      Behaviour current_behaviour = {};
    };
  }
}
