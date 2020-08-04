#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "afk/NumericTypes.hpp"
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
      f32 duration      = {};
      f32 frame_rate    = {};
      Channels channels = {};
    };
  }
}
