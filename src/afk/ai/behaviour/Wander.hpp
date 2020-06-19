#pragma once

#include "afk/ai/behaviour/BaseBehaviour.hpp"

namespace Afk {
  namespace AI {
    /**
     * Random wandering behaviour
     * \todo Fix implementation of this to match impl from lecture slides as this version doesn't work well
     */
    class Wander : public BaseBehaviour {
    public:
      auto update(const glm::vec3 &current_position) -> glm::vec3 override;
      Wander(const glm::vec3 &target, float wander_radius);
      ~Wander() override = default;

    private:
      glm::vec3 center;
      glm::vec3 last_wander;
      float range;
    };
  }
}
