#pragma once

#include "afk/config/Config.hpp"

namespace afk {
  namespace config {
    /**
     * Manages the engine configuration.
     */
    class ConfigManager {
    public:
      /** The engine configuration directory. */
      static constexpr const char8_t *CONFIG_DIR = u8"cfg";
      /** The engine configuration file path. */
      static constexpr const char8_t *CONFIG_FILE_PATH = u8"cfg/engine.json";

      /** The engine config. */
      Config config = {};

      /**
       * Initializes this config manager.
       */
      auto initialize() -> void;

    private:
      /** Is the renderer initialized? */
      bool is_initialized = false;

      /**
       * Loads the engine configuration file.
       */
      auto load_config_file() -> void;
    };
  }
}
