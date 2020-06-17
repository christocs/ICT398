#include "afk/Afk.hpp"

#include <memory>
#include <string>
#include <utility>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "afk/asset/AssetFactory.hpp"
#include "afk/component/AgentComponent.hpp"
#include "afk/component/GameObject.hpp"
#include "afk/component/ScriptsComponent.hpp"
#include "afk/debug/Assert.hpp"
#include "afk/io/Log.hpp"
#include "afk/io/ModelSource.hpp"
#include "afk/physics/PhysicsBody.hpp"
#include "afk/physics/RigidBodyType.hpp"
#include "afk/physics/shape/Box.hpp"
#include "afk/physics/shape/Sphere.hpp"
#include "afk/renderer/ModelRenderSystem.hpp"
#include "afk/script/Bindings.hpp"
#include "afk/script/LuaInclude.hpp"

using namespace std::string_literals;

using glm::vec3;
using glm::vec4;

using Afk::Engine;
using Afk::Event;
using Afk::Texture;
using Action   = Afk::Event::Action;
using Movement = Afk::Camera::Movement;

auto Engine::initialize() -> void {
  afk_assert(!this->is_initialized, "Engine already initialized");

  this->renderer.initialize();
  this->event_manager.initialize(this->renderer.window);
  //  this->renderer.set_wireframe(true);

  // load the navmesh before adding components to things
  // init crowds with the navmesh
  // then add components

  this->ui.initialize(this->renderer.window);
  this->lua = luaL_newstate();
  luaL_openlibs(this->lua);
  Afk::add_engine_bindings(this->lua);

  this->terrain_manager.initialize();
  const int terrain_width  = 128;
  const int terrain_length = 128;
  this->terrain_manager.generate_terrain(terrain_width, terrain_length, 0.05f, 7.5f);
  this->renderer.load_model(this->terrain_manager.get_model());

  auto terrain_entity           = registry.create();
  auto terrain_transform        = Transform{terrain_entity};
  terrain_transform.translation = glm::vec3{0.0f, -10.0f, 0.0f};
  registry.assign<Afk::ModelSource>(terrain_entity, terrain_entity,
                                    terrain_manager.get_model().file_path,
                                    "shader/terrain.prog");
  registry.assign<Afk::Model>(terrain_entity, terrain_entity, terrain_manager.get_model());

  registry.assign<Afk::Transform>(terrain_entity, terrain_transform);
  registry.assign<Afk::PhysicsBody>(terrain_entity, terrain_entity, &this->physics_body_system,
                                    terrain_transform, 0.3f, 0.0f, 0.0f, 0.0f,
                                    true, Afk::RigidBodyType::STATIC,
                                    this->terrain_manager.height_map);

  auto box_entity           = registry.create();
  auto box_transform        = Transform{box_entity};
  box_transform.translation = glm::vec3{0.0f, -15.0f, 0.0f};
  box_transform.scale       = glm::vec3(5.0f);
  auto box_model            = Model(box_entity, "res/model/box/box.obj");
  this->renderer.load_model(box_model);
  registry.assign<Afk::ModelSource>(box_entity, box_entity, box_model.file_path,
                                    "shader/default.prog");
  registry.assign<Afk::Model>(box_entity, box_model);
  registry.assign<Afk::Transform>(box_entity, box_transform);
  registry.assign<Afk::PhysicsBody>(
      box_entity, box_entity, &this->physics_body_system, box_transform, 0.3f, 0.0f,
      0.0f, 0.0f, true, Afk::RigidBodyType::STATIC, Afk::Box(0.9f, 0.9f, 0.9f));

  // auto basketball =
  // std::get<Asset::Asset::Object>(
  Afk::Asset::game_asset_factory("asset/basketball.lua"); //.data)
                                                          // .ent;

  this->nav_mesh_manager.initialise("res/gen/navmesh/human.nmesh", &this->registry);
  //  this->nav_mesh_manager.initialise("res/gen/navmesh/solo_navmesh.bin", this->terrain_manager.get_model().meshes[0], terrain_transform);
  this->crowds.init(this->nav_mesh_manager.get_nav_mesh());

  /*
    this->renderer.load_model(this->nav_mesh_manager.get_height_field_model());
    auto height_field_entity = registry.create();
    auto height_field_transform = Transform{height_field_entity};
    height_field_transform.translation = glm::vec3(0.0f); // zero out translation, translation should already be matched with the terrain
    registry.assign<Afk::ModelSource>(
        height_field_entity, height_field_entity,
        this->nav_mesh_manager.get_height_field_model().file_path, "shader/heightfield.prog");
    registry.assign<Afk::Transform>(height_field_entity, height_field_transform);
    /**/

  this->renderer.load_model(this->nav_mesh_manager.get_nav_mesh_model());
  auto nav_mesh_entity    = registry.create();
  auto nav_mesh_transform = Transform{nav_mesh_entity};
  nav_mesh_transform.translation =
      glm::vec3(0.0f); // zero out translation, translation should already be matched with the terrain
  registry.assign<Afk::ModelSource>(nav_mesh_entity, nav_mesh_entity,
                                    this->nav_mesh_manager.get_nav_mesh_model().file_path,
                                    "shader/navmesh.prog");
  registry.assign<Afk::Transform>(nav_mesh_entity, nav_mesh_transform);

  auto cam = registry.create();
  registry.assign<Afk::Transform>(cam, cam);
  registry.assign<Afk::ScriptsComponent>(cam, cam, this->lua)
      .add_script("script/component/camera_keyboard_control.lua", &this->event_manager)
      .add_script("script/component/camera_mouse_control.lua", &this->event_manager)
      .add_script("script/component/debug.lua", &this->event_manager);

  std::vector<entt::entity> agents{};
  for (std::size_t i = 0; i < 5; ++i) {
    agents.push_back(registry.create());
    dtCrowdAgentParams p        = {};
    p.radius                    = .1f;
    p.maxSpeed                  = 1;
    p.maxAcceleration           = 1;
    p.height                    = 1;
    auto agent_transform        = Afk::Transform{agents[i]};
    agent_transform.translation = {5 - (i), -6, 5 - (i)};
    agent_transform.scale       = {.1f, .1f, .1f};
    registry.assign<Afk::Transform>(agents[i], agent_transform);
    registry.assign<Afk::ModelSource>(agents[i], agents[i], "res/model/nanosuit/nanosuit.fbx",
                                      "shader/default.prog");
    auto &agent_component = registry.assign<Afk::AI::AgentComponent>(
        agents[i], agents[i], agent_transform.translation, p);
  }
  // registry.get<Afk::AI::AgentComponent>(agents[0]).move_to({25, -5, 25});
  // registry.get<Afk::AI::AgentComponent>(agents[1]).chase(cam, 10.f);
  // registry.get<Afk::AI::AgentComponent>(agents[2]).flee(cam, 10.f);
  // const Afk::AI::Path path = {{2.8f, -9.f, 3.f}, {14.f, -8.f, 4.f}, {20.f, -10.f, -3.5f}};
  // registry.get<Afk::AI::AgentComponent>(agents[3]).path(path, 2.f);
  registry.get<Afk::AI::AgentComponent>(agents[4]).wander(glm::vec3{0, 0, 0}, 10);

  this->is_initialized = true;
}

auto Engine::get() -> Engine & {
  static auto instance = Engine{};

  return instance;
}

auto Engine::exit() -> void {
  lua_close(this->lua);
  this->is_running = false;
}

auto Engine::render() -> void {
  Afk::queue_models(&this->registry, &this->renderer);

  this->renderer.clear_screen({135.0f, 206.0f, 235.0f, 1.0f});
  this->ui.prepare();
  this->renderer.draw();
  this->ui.draw();
  this->renderer.swap_buffers();
}

auto Engine::update() -> void {
  this->event_manager.pump_events();
  this->crowds.update(this->get_delta_time());
  for (auto &agent_ent : this->registry.view<Afk::AI::AgentComponent>()) {
    auto &agent = this->registry.get<Afk::AI::AgentComponent>(agent_ent);
    agent.update();
  }

  if (glfwWindowShouldClose(this->renderer.window)) {
    this->is_running = false;
  }

  if (this->ui.show_menu) {
    glfwSetInputMode(this->renderer.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  } else {
    glfwSetInputMode(this->renderer.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }

  // this->update_camera();

  this->physics_body_system.update(&this->registry, this->get_delta_time());

  ++this->frame_count;
  this->last_update = Afk::Engine::get_time();
}

auto Engine::get_time() -> float {
  return static_cast<float>(glfwGetTime());
}

auto Engine::get_delta_time() -> float {
  return this->get_time() - this->last_update;
}

auto Engine::get_is_running() const -> bool {
  return this->is_running;
}
