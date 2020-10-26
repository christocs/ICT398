#pragma once

#include <string>
#include <unordered_map>

using std::string;

namespace afk {
  namespace ecs {
    namespace component {
      // Represents both needs and the affordances to sate these needs
      struct Needs {
        /**
         * rest
         */
        const static string sit;
        /**
         * hunger
         */
        const static string eat;
        /**
         * sudden urge for violence
         */
        const static string kick;

        std::unordered_map<string, float> need;

        Needs &operator-=(const Needs &right);
        Needs &operator+=(const Needs &right);
        float operator[](const string &index);
      };
    }
  }
}
