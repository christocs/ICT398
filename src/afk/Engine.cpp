#include "afk/Engine.hpp"

#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include "afk/config/Config.hpp"
#include "afk/debug/Assert.hpp"
#include "afk/ecs/system/PhysicsSystem.hpp"
#include "afk/io/Json.hpp"
#include "afk/io/JsonSerialization.hpp"
#include "afk/io/Log.hpp"
#include "afk/io/Path.hpp"
#include "afk/io/Time.hpp"
#include "afk/io/Unicode.hpp"
#include "afk/render/Model.hpp"
#include "afk/render/Renderer.hpp"

using namespace std::string_literals;

using afk::Engine;
using afk::config::Config;
using afk::config::ConfigManager;
using afk::event::Event;
using afk::io::Json;
using std::filesystem::path;

auto Engine::get() -> Engine & {
  static auto instance = Engine{};

  if (!instance.is_initialized) {
    instance.initialize();
  }

  return instance;
}

auto Engine::initialize() -> void {
  afk_assert(!this->is_initialized, "Engine already initialized");
  this->is_initialized = true;
  afk::io::create_engine_dirs();
  afk::io::log << afk::io::get_date_time() << "afk engine starting...\n";
  this->config_manager.initialize();
  this->ecs.initialize();
  this->renderer.initialize();
  this->event_manager.initialize(this->renderer.window);
  this->ui_manager.initialize(this->renderer.window);
  this->collision_system.initialize();
  this->physics_system.initialize();
  this->prefab_manager.initialize();
  this->scene_manager.initialize();

  this->event_manager.register_event(Event::Type::MouseMove,
                                     event::EventManager::Callback{[this](Event event) {
                                       this->move_mouse(event);
                                     }});

  this->event_manager.register_event(
      Event::Type::KeyDown, event::EventManager::Callback{[this](Event event) {
        this->move_keyboard(event);
      }});

  this->scene_manager.instantiate_scene("default");
}

auto Engine::render() -> void {
  this->renderer.clear_screen({135.0f, 206.0f, 235.0f, 1.0f});
  this->ecs.system_manager.display_update();

  auto mesh_model_transform        = physics::Transform{};
  mesh_model_transform.translation = glm::vec3{0.0f, 0.0f, 0.0f};
  mesh_model_transform.rotation    = glm::identity<glm::quat>();

  if (this->display_debug_physics_mesh) {
    auto debug_mesh = this->collision_system.get_debug_mesh();
    if (!debug_mesh.vertices.empty()) {
      const auto old_wireframe_status = this->renderer.get_wireframe();
      if (!old_wireframe_status) {
        this->renderer.set_wireframe(true);
      }
      static int debug_mesh_count = 0;

      auto debug_mesh_model = render::Model();
      ++debug_mesh_count;
      debug_mesh_model.meshes = {std::move(debug_mesh)};
      debug_mesh_model.file_path =
          afk::io::get_resource_path().string() + "/debug/";
      debug_mesh_model.file_path =
          debug_mesh_model.file_path.string() + std::to_string(debug_mesh_count);

      auto debug_mesh_model_handle = this->renderer.load_model(debug_mesh_model);
      const auto shader =
          this->renderer.get_shader_program("res/shader/rp3dmesh.prog");
      this->renderer.draw_model(debug_mesh_model_handle, shader, mesh_model_transform);

      if (!old_wireframe_status) {
        this->renderer.set_wireframe(false);
      }
    }
  }

  this->ui_manager.prepare();
  this->ui_manager.draw();

  this->renderer.swap_buffers();
}

auto Engine::update() -> void {
  this->collision_system.update();
  this->physics_system.update();
  this->ecs.system_manager.update();
  this->event_manager.pump_events();

  if (glfwWindowShouldClose(this->renderer.window.get())) {
    this->is_running = false;
  }

  if (this->ui_manager.show_menu) {
    glfwSetInputMode(this->renderer.window.get(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  } else {
    glfwSetInputMode(this->renderer.window.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }

  ++this->frame_count;
  this->last_update = afk::Engine::get_time();
}

auto Engine::exit() -> void {
  afk::io::log << afk::io::get_date_time() << "afk engine quitting...\n";
  const auto config_path =
      afk::io::get_resource_path(afk::io::to_cstr(ConfigManager::CONFIG_FILE_PATH));
  auto json = Json{};
  json      = this->config_manager.config;
  afk::io::write_json_to_file(config_path, json);

  this->is_running = false;
}

auto Engine::get_time() -> f32 {
  return static_cast<f32>(glfwGetTime());
}

auto Engine::get_delta_time() -> f32 {
  return this->get_time() - this->last_update;
}

auto Engine::get_is_running() const -> bool {
  return this->is_running;
}

auto Engine::move_mouse(Event event) -> void {
  const auto data = std::get<Event::MouseMove>(event.data);

  static auto last_x      = 0.0f;
  static auto last_y      = 0.0f;
  static auto first_frame = true;

  const auto dx = static_cast<f32>(data.x) - last_x;
  const auto dy = static_cast<f32>(data.y) - last_y;

  if (!first_frame && !this->ui_manager.show_menu) {
    this->camera.handle_mouse(dx, dy);
  } else {
    first_frame = false;
  }

  last_x = static_cast<f32>(data.x);
  last_y = static_cast<f32>(data.y);
}

auto Engine::move_keyboard(Event event) -> void {
  const auto key = std::get<Event::Key>(event.data).key;

  if (event.type == Event::Type::KeyDown && key == GLFW_KEY_ESCAPE) {
    this->exit();
  } else if (event.type == Event::Type::KeyDown && key == GLFW_KEY_GRAVE_ACCENT) {
    this->ui_manager.show_menu = !this->ui_manager.show_menu;
  } else if (event.type == Event::Type::KeyDown && key == GLFW_KEY_1) {
    this->renderer.set_wireframe(!this->renderer.get_wireframe());
  }
}
