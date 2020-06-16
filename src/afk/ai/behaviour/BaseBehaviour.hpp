#pragma once

#include <glm/glm.hpp>

namespace Afk {
  namespace AI {
    class BaseBehaviour {
    public:
      virtual auto update(const glm::vec3 &current_position) -> glm::vec3 = 0;

    protected:
      BaseBehaviour()          = default;
      virtual ~BaseBehaviour() = default;
    };
  }
}
