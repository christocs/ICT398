#pragma once

#include <glm/glm.hpp>

namespace Afk {
  namespace AI {
    /**
     * Base AI movement behaviour
     */
    class BaseBehaviour {
    public:
      /**
       * Get a new position based on the current position
       */
      virtual auto update(const glm::vec3 &current_position) -> glm::vec3 = 0;

    protected:
      BaseBehaviour()          = default;
      virtual ~BaseBehaviour() = default;
    };
  }
}
