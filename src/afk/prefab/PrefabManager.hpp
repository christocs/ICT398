#pragma once

#include <functional>
#include <string>
#include <unordered_map>

#include "afk/ecs/Entity.hpp"
#include "afk/io/Json.hpp"
#include "afk/io/Unicode.hpp"
#include "afk/prefab/Prefab.hpp"

namespace afk {
  namespace prefab {
    /**
     * Manages prefabs.
     */
    class PrefabManager {
    public:
      /** Map of prefab names to prefabs. */
      using PrefabMap = std::unordered_map<std::string, Prefab>;
      /** Map of component names to components. */
      using ComponentMap = std::unordered_map<std::string, afk::ecs::component::Component>;

      /** The path to load prefabs from. */
      static constexpr const auto *PREFAB_PATH = u8"res/prefab";
      /** The map of known component names to components. */
      static inline const auto COMPONENT_MAP =
          ComponentMap{{"Model", afk::ecs::component::ModelComponent{}},
                       {"Transform", afk::ecs::component::TransformComponent{}},
                       {"Velocity", afk::ecs::component::VelocityComponent{}},
                       {"Collider", afk::ecs::component::ColliderComponent{}}};

      /**
       * Instantiates a prefab by name and returns the created entity.
       *
       * Assumes prefab is not already instantiated
       *
       * @param name The prefab name.
       * @return The created entity.
       */
      auto instantiate_prefab(const std::string &name) const -> afk::ecs::Entity;

      /**
       * Instantiates the specified  prefab and returns the created entity.
       *
       * @param prefab The prefab to instantiate.
       * @return The created entity.
       */
      auto instantiate_prefab(const Prefab &prefab) const -> afk::ecs::Entity;

      /**
       * Initializes this config manager.
       */
      auto initialize() -> void;

      /** Map of the loaded prefabs. */
      PrefabMap prefab_map = {};

    private:
      /** Is the renderer initialized? */
      bool is_initialized = false;

      /**
       * Loads all prefab files inside specified path.
       *
       * @param dir_path The prefab directory path.
       */
      auto load_prefabs_from_dir(const std::filesystem::path &dir_path = afk::io::to_cstr(PREFAB_PATH))
          -> void;

      /**
       * Initializes a component. Some components require additional data which
       * can only be obtained at runtime. For example, the ModelComponent
       * requires the loaded model handle in order to draw.
       *
       * @param json The json used to create the component.
       * @param component The component to initialize.
       */
      static auto initialize_component(const afk::io::Json &json,
                                       afk::ecs::component::Component &component) -> void;
    };
  }
}
