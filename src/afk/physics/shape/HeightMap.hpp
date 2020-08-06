#pragma once

#include <vector>

namespace afk {
  namespace physics {
    namespace shape {
      /**
       * Height Map shape
       */
      struct HeightMap {
        struct Point {
          int x = {};
          int y = {};
        };

        std::vector<float> heights = {};
        int width                  = {};

        auto at(Point p) const -> float;
        auto operator[](Point p) -> float &;
      };
    }
  }
}
