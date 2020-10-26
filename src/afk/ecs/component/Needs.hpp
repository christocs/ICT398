#pragma once

namespace afk {
  namespace ecs {
    namespace component {
      // Represents both needs and the affordances to sate these needs
      struct Needs {
        /**
         * rest - 0-1
         */
        float sit = 0;
        /**
         * hunger - 0-1
         */
        float eat = 0;
        /**
         * sudden urge for violence - 0-1
         */
        float kick = 0;

        Needs &operator-=(const Needs &right);
        Needs &operator+=(const Needs &right);
      };
    }
  }
}
