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
#include "afk/io/Unicode.hpp"
#include "afk/prefab/PrefabManager.hpp"
#include "afk/scene/Scene.hpp"
#include "afk/utility/Visitor.hpp"

using afk::io::Json;
using afk::prefab::Prefab;
using afk::prefab::PrefabManager;
using afk::scene::Scene;
using afk::scene::SceneManager;
using afk::utility::Visitor;
using namespace afk::ecs::component;
using std::ifstream;
using std::string;
using std::filesystem::directory_iterator;
using std::filesystem::path;
using namespace std::string_literals;

auto SceneManager::load_scenes_from_dir(const path &dir_path) -> void {
  const auto scene_dir = afk::io::get_resource_path(dir_path);

  auto &afk = afk::Engine::get();

  for (const auto &file_path : directory_iterator{scene_dir}) {
    auto file  = ifstream{file_path};
    auto json  = Json{};
    auto scene = Scene{};
    file >> json;

    afk_assert(file.is_open(), "Unable to open scene file "s + file_path.path().string());

    scene.name    = json.at("name");
    auto entities = json.at("entities");

    for (const auto &[_, entity_value] : entities.items()) {
      auto prefab = afk.prefab_manager.prefab_map.at(entity_value.at("name"));

      for (const auto &[component_name, component_value] :
           entity_value.at("components").items()) {
        auto component      = PrefabManager::COMPONENT_MAP.at(component_name);
        auto component_json = component_value;

        auto visitor = Visitor{
            [&component_json](ModelComponent &c) { c = component_json; },
            [&component_json](PositionComponent &c) { c = component_json; },
            [&component_json](VelocityComponent &c) { c = component_json; },
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

    afk::io::log << "Loaded scene "
                 << file_path.path().lexically_relative(afk::io::get_resource_path())
                 << "\n";
  }
}

auto SceneManager::initialize() -> void {
  afk_assert(!this->is_initialized, "Scene manager already initialized");
  this->load_scenes_from_dir();
  this->is_initialized = true;
}

auto SceneManager::load_scene(const std::string &name) const -> void {
  auto &afk         = afk::Engine::get();
  const auto &scene = this->scene_map.at(name);

  for (const auto &prefab : scene.prefabs) {
    afk.prefab_manager.instantiate_prefab(prefab);
  }
}
