#pragma once

namespace afk {
  namespace ecs {
    namespace component {
      // Represents both needs and the affordances to sate these needs
      struct Needs {
        /**
         * rest
         */
        float sit = 0;
        /**
         * hunger
         */
        float eat = 0;
        /**
         * sudden urge for violence
         */
        float kick = 0;

        Needs &operator-=(const Needs &right);
      };
    }
  }
}
