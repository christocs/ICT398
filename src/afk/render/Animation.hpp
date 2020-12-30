#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "afk/NumericTypes.hpp"
#include "afk/physics/Transform.hpp"

namespace afk {
  namespace render {
    /**
     * Encapsulates a model animation.
     */
    struct Animation {
      /**
       * Encapsulates an animation channel.
       */
      struct Channel {
        /** A collection of keyframes. */
        using Frames = std::vector<physics::Transform>;

        /** The channel name. */
        std::string name = {};
        /** The keyframes. */
        Frames frames = {};
      };

      /** A collection of channels. */
      using Channels = std::vector<Channel>;

      /** The animation name. */
      std::string name = {};
      /** The animation duration, in seconds. */
      f32 duration = {};
      /** The animation frame rate. */
      f32 frame_rate = {};
      /** The animation channels. */
      Channels channels = {};
    };
  }
}
