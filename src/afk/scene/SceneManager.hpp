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
       * Initializes this scene manager.
       */
      auto initialize() -> void;

      /**
       * Instantiates all prefabs contained the specified scene and destroys all current entities.
       *
       * @param name The scene name.
       */
      auto instantiate_scene(const std::string &name) const -> void;

      /** The map of loaded scenes. */
      SceneMap scene_map = {};

    private:
      /** Is the scene manager initialized? */
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
