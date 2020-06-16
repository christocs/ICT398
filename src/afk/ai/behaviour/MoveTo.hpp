#pragma once

#include "afk/ai/behaviour/BaseBehaviour.hpp"

namespace Afk {
  namespace AI {
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
