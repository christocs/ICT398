#include "afk/prefab/PrefabManager.hpp"

#include <filesystem>
#include <fstream>
#include <string>

#include <glm/glm.hpp>

#include "afk/debug/Assert.hpp"
#include "afk/ecs/component/Component.hpp"
#include "afk/io/Json.hpp"
#include "afk/io/JsonSerialization.hpp"
#include "afk/io/Log.hpp"
#include "afk/io/Path.hpp"
#include "afk/io/Time.hpp"
#include "afk/io/Unicode.hpp"
#include "afk/prefab/Prefab.hpp"
#include "afk/utility/Visitor.hpp"

using std::ifstream;
using std::string;
using std::filesystem::directory_iterator;
using std::filesystem::path;
using namespace std::string_literals;

using afk::ecs::Entity;
using afk::ecs::component::Component;
using afk::io::Json;
using afk::prefab::Prefab;
using afk::prefab::PrefabManager;
using afk::utility::Visitor;
using namespace afk::ecs::component;

auto PrefabManager::load_prefabs_from_dir(const path &dir_path) -> void {
  const auto prefab_dir = afk::io::get_resource_path(dir_path);

  for (const auto &entry : directory_iterator{prefab_dir}) {
    const auto path = entry.path();
    auto file       = ifstream{path};
    auto json       = Json{};
    auto prefab     = Prefab{};

    afk_assert(file.is_open(), "Unable to open prefab file "s + path.string());

    file >> json;

    prefab.name     = json.at("name").get<string>();
    auto components = json.at("components");

    for (const auto &[component_name, component_json] : components.items()) {
      const auto &j  = component_json;
      auto component = this->COMPONENT_MAP.at(component_name);

      auto visitor =
          Visitor{[j](ModelComponent &c) { c = j.get<ModelComponent>(); },
                  [j](TransformComponent &c) { c = j.get<TransformComponent>(); },
                  [j](VelocityComponent &c) { c = j.get<VelocityComponent>(); },
                  [j](ColliderComponent &c) { c = j.get<ColliderComponent>(); },
                  [](auto) { afk_unreachable(); }};

      std::visit(visitor, component);

      afk_assert(prefab.components.find(component_name) == prefab.components.end(),
                 "Component already exists");
      this->initialize_component(components.at(component_name), component);
      prefab.components[component_name] = std::move(component);
    }

    afk_assert(this->prefab_map.find(prefab.name) == this->prefab_map.end(),
               "Prefab already exists");
    this->prefab_map[prefab.name] = std::move(prefab);
    afk::io::log << afk::io::get_date_time() << "Loaded prefab "
                 << path.lexically_relative(afk::io::get_resource_path()) << '\n';
  }
}

auto PrefabManager::initialize_component(const Json &json, Component &component) -> void {
  auto visitor = Visitor{[json](ModelComponent &c) {
                           auto &afk = afk::Engine::get();
                           auto path = afk::io::get_resource_path(
                               json.at("file_path").get<string>());
                           c.model_handle = afk.renderer.get_model(path);
                         },
                         [](auto) {}};

  std::visit(visitor, component);
}

auto PrefabManager::instantiate_prefab(const Prefab &prefab) const -> Entity {
  auto &afk      = afk::Engine::get();
  auto &registry = afk.ecs.registry;

  auto entity = registry.create();

  auto visitor = Visitor{[&registry, entity](ModelComponent component) {
                           registry.emplace<ModelComponent>(entity, component);
                         },
                         [&registry, entity](TransformComponent component) {
                           registry.emplace<TransformComponent>(entity, component);
                         },
                         [&registry, entity](VelocityComponent component) {
                           registry.emplace<VelocityComponent>(entity, component);
                         },
                         [&registry, entity, &prefab, &afk](ColliderComponent component) {
                           afk_assert(prefab.components.count("Transform") == 1, "prefab must have a Transform component to instantiate a collider component");
                           // check that the "Transform" component is a transform component, then use it when instantiating the collider
                           auto transform_visitor =
                               Visitor{[entity, &afk, &component](TransformComponent transform) {
                                         afk.collision_system.instantiate_collider(
                                             entity, component, transform);
                                       },
                                       [](auto) { afk_unreachable(); }};

                           std::visit(transform_visitor, prefab.components.at("Transform"));

                           registry.emplace<ColliderComponent>(entity, component);
                         },
                         [](auto) { afk_unreachable(); }};

  for (const auto &[_, component] : prefab.components) {
    std::visit(visitor, component);
  }

  return entity;
}

auto PrefabManager::instantiate_prefab(const string &name) const -> Entity {
  return this->instantiate_prefab(this->prefab_map.at(name));
}

auto PrefabManager::initialize() -> void {
  afk_assert(!this->is_initialized, "Prefab manager already initialized");
  this->load_prefabs_from_dir();
  this->is_initialized = true;
  afk::io::log << afk::io::get_date_time() << "Prefab subsystem initialized\n";
}
