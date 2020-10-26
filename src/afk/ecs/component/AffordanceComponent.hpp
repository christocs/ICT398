#pragma once
#include "afk/ecs/component/Needs.hpp"

namespace afk {
  namespace ecs {
    namespace component {
      struct AffordanceComponent {
        // Affordances provided by this object
        Needs affords;
      };
    }
  }
}
