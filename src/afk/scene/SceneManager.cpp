#include "afk/scene/SceneManager.hpp"

#include <filesystem>
#include <fstream>
#include <string>

#include "afk/Engine.hpp"
#include "afk/debug/Assert.hpp"
#include "afk/ecs/component/Component.hpp"
#include "afk/io/Json.hpp"
#include "afk/io/JsonSerialization.hpp"
#include "afk/io/Log.hpp"
#include "afk/io/Path.hpp"
#include "afk/io/Time.hpp"
#include "afk/io/Unicode.hpp"
#include "afk/prefab/PrefabManager.hpp"
#include "afk/scene/Scene.hpp"
#include "afk/utility/Visitor.hpp"

using std::ifstream;
using std::string;
using std::filesystem::directory_iterator;
using std::filesystem::path;
using namespace std::string_literals;

using afk::io::Json;
using afk::prefab::Prefab;
using afk::prefab::PrefabManager;
using afk::scene::Scene;
using afk::scene::SceneManager;
using afk::utility::Visitor;
using namespace afk::ecs::component;

auto SceneManager::load_scenes_from_dir(const path &dir_path) -> void {
  const auto scene_dir = afk::io::get_resource_path(dir_path);

  auto &afk = afk::Engine::get();

  for (const auto &entry : directory_iterator{scene_dir}) {
    const auto path = entry.path();
    auto file       = ifstream{path};
    auto json       = Json{};
    auto scene      = Scene{};
    file >> json;

    afk_assert(file.is_open(), "Unable to open scene file "s + path.string());

    scene.name    = json.at("name").get<string>();
    auto entities = json.at("entities");

    for (const auto &[_, entity_json] : entities.items()) {
      auto prefab =
          afk.prefab_manager.prefab_map.at(entity_json.at("name").get<string>());

      for (const auto &[component_name, component_json] :
           entity_json.at("components").items()) {
        const auto &j  = component_json;
        auto component = PrefabManager::COMPONENT_MAP.at(component_name);

        auto visitor =
            Visitor{[j](ModelComponent &c) { c = j.get<ModelComponent>(); },
                    [j](TransformComponent &c) { c = j.get<TransformComponent>(); },
                    [j](VelocityComponent &c) { c = j.get<VelocityComponent>(); },
                    [](auto) { afk_unreachable(); }};

        std::visit(visitor, component);

        afk_assert(prefab.components.find(component_name) != prefab.components.end(),
                   "Prefab missing component in entity");
        prefab.components[component_name] = component;
      }

      scene.prefabs.push_back(prefab);
    }

    afk_assert(this->scene_map.find(scene.name) == this->scene_map.end(),
               "Scene already exists");
    this->scene_map[scene.name] = std::move(scene);

    afk::io::log << afk::io::get_date_time() << "Loaded scene "
                 << path.lexically_relative(afk::io::get_resource_path()) << '\n';
  }
}

auto SceneManager::initialize() -> void {
  afk_assert(!this->is_initialized, "Scene manager already initialized");
  this->load_scenes_from_dir();
  this->is_initialized = true;
  afk::io::log << afk::io::get_date_time() << "Scene subsystem initialized\n";
}

auto SceneManager::instantiate_scene(const std::string &name) const -> void {
  auto &afk         = afk::Engine::get();
  const auto &scene = this->scene_map.at(name);

  for (const auto &prefab : scene.prefabs) {
    afk.prefab_manager.instantiate_prefab(prefab);
  }

  afk::io::log << afk::io::get_date_time() << "Instantiated scene \"" << name << "\"\n";
}
