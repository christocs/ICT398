#pragma once

#include "afk/ai/behaviour/BaseBehaviour.hpp"

namespace Afk {
  namespace AI {
    /**
     * Behaviour to move to specific coordinates
     */
    class MoveTo : public BaseBehaviour {
    public:
      auto update(const glm::vec3 &current_position) -> glm::vec3 override;
      MoveTo(const glm::vec3 &target);
      ~MoveTo() override = default;

    private:
      glm::vec3 target;
    };
  }
}
