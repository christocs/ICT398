#pragma once

#include "afk/ai/behaviour/BaseBehaviour.hpp"
#include "afk/component/GameObject.hpp"

namespace Afk {
  namespace AI {
    class Chase : public BaseBehaviour {
    public:
      auto update(const glm::vec3 &current_position) -> glm::vec3 override;
      Chase(const GameObject &target, float max_distance);
      ~Chase() override = default;

    private:
      GameObject target = entt::null;
      float max_dist    = {};
    };
  }
}
