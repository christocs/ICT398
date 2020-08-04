#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "afk/physics/Transform.hpp"

namespace afk {
  namespace render {
    struct Animation {
      struct Channel {
        using Frames = std::vector<physics::Transform>;

        std::string name = {};
        Frames frames    = {};
      };

      using Channels = std::vector<Channel>;

      std::string name  = {};
      float duration    = {};
      float frame_rate  = {};
      Channels channels = {};
    };
  }
}
