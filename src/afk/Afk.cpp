#include "afk/Afk.hpp"

#include <memory>
#include <string>
#include <utility>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "afk/asset/AssetFactory.hpp"
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

  this->ui.initialize(this->renderer.window);
  this->lua = luaL_newstate();
  luaL_openlibs(this->lua);
  Afk::add_engine_bindings(this->lua);

  this->terrain_manager.initialize();
  const int terrain_width  = 1024;
  const int terrain_length = 1024;
  this->terrain_manager.generate_terrain(terrain_width, terrain_length, 0.05f, 7.5f);
  this->renderer.load_model(this->terrain_manager.get_model());

  auto terrain_entity           = registry.create();
  auto terrain_transform        = Transform{terrain_entity};
  terrain_transform.translation = glm::vec3{0.0f, -10.0f, 0.0f};
  registry.assign<Afk::ModelSource>(terrain_entity, terrain_entity,
                                    terrain_manager.get_model().file_path,
                                    "shader/terrain.prog");
  registry.assign<Afk::Transform>(terrain_entity, terrain_entity);
  registry.assign<Afk::PhysicsBody>(terrain_entity, terrain_entity, &this->physics_body_system,
                                    terrain_transform, 0.3f, 0.0f, 0.0f, 0.0f,
                                    true, Afk::RigidBodyType::STATIC,
                                    this->terrain_manager.height_map);

  Afk::Asset::game_asset_factory("asset/basketball.lua");

  auto cam = registry.create();
  registry.assign<Afk::ScriptsComponent>(cam, cam, this->lua)
      .add_script("script/component/camera_keyboard_control.lua", &this->event_manager)
      .add_script("script/component/camera_mouse_control.lua", &this->event_manager)
      .add_script("script/component/debug.lua", &this->event_manager);

  this->is_initialized = true;
}

auto Engine::get() -> Engine & {
  static auto instance = Engine{};

  return instance;
}

auto Engine::exit() -> void {
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
