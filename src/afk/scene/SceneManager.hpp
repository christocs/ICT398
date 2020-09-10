#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

#include "afk/io/Unicode.hpp"
#include "afk/scene/Scene.hpp"

namespace afk {
  namespace scene {
    class SceneManager {
    public:
      /** Maps scene names to scenes. */
      using SceneMap = std::unordered_map<std::string, afk::scene::Scene>;

      /** The path to load scenes from. */
      static constexpr const auto *SCENE_DIR = u8"res/scene";

      /**
       * Instantiates all prefabs contained the specified scene.
       *
       * @param name The scene name.
       */
      auto load_scene(const std::string &name) const -> void;

      /**
       * Initializes this config manager.
       */
      auto initialize() -> void;

      /** The map of loaded scenes. */
      SceneMap scene_map = {};

    private:
      /** Is the renderer initialized? */
      bool is_initialized = false;

      /**
       * Loads all prefab files inside specified path.
       *
       * @param dir_path The prefab directory path.
       */
      auto load_scenes_from_dir(const std::filesystem::path &dir_path = afk::io::to_cstr(SCENE_DIR))
          -> void;
    };
  }
}
