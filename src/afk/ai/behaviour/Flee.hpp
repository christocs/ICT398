#pragma once

#include "afk/ai/behaviour/BaseBehaviour.hpp"
#include "afk/component/GameObject.hpp"

namespace Afk {
  namespace AI {
    /**
     * Flee behaviour - move desired_distance away from entity
     */
    class Flee : public BaseBehaviour {
    public:
      auto update(const glm::vec3 &current_position) -> glm::vec3 override;
      Flee(const GameObject &target, float desired_distance);
      ~Flee() override = default;

    private:
      GameObject target      = entt::null;
      float desired_distance = {};
    };
  }
}
