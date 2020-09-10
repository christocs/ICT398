#pragma once

#include <filesystem>
#include <string>

#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

#include "afk/NumericTypes.hpp"

namespace afk {
  namespace config {
    /**
     * Encapsulates the video configuration options.
     */
    struct Video {
      /** The resolution width. */
      i32 resolution_width = 1920;
      /** The resolution height. */
      i32 resolution_height = 1080;
      /** Is the game fullscreen or windowed? */
      bool fullscreen_enabled = true;
      /** How many anti-aliasing samples to use. */
      i32 antialiasing_samples = 4;
      /** Is anti-aliasing enabled? */
      bool antialiasing_enabled = true;
      /** Is vsync enabled? */
      bool vsync_enabled = true;
    };

    /**
     * Encapsulates the engine configuration options.
     */
    struct Config {
      /** The video configuration. */
      Video video = {};
    };
  }
}
