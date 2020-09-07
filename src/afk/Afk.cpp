#include "afk/Afk.hpp"

#include <memory>
#include <string>
#include <utility>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "afk/debug/Assert.hpp"
#include "afk/io/Log.hpp"
#include "afk/render/Renderer.hpp"

using namespace std::string_literals;

using glm::vec3;
using glm::vec4;

using afk::Engine;
using afk::event::Event;
using afk::physics::Transform;
using afk::render::Texture;
using Movement = afk::render::Camera::Movement;

auto Engine::get() -> Engine & {
  static auto instance = Engine{};

  return instance;
}

auto Engine::initialize() -> void {
  afk_assert(!this->is_initialized, "Engine already initialized");

  this->renderer.initialize();
  this->event_manager.initialize(this->renderer.window);
  this->ui_manager.initialize(this->renderer.window);

  this->event_manager.register_event(Event::Type::MouseMove,
                                     event::EventManager::Callback{[this](Event event) {
                                       this->move_mouse(event);
                                     }});
  this->event_manager.register_event(
      Event::Type::KeyDown, event::EventManager::Callback{[this](Event event) {
        this->move_keyboard(event);
      }});
//  this->event_manager.register_event(
//      afk::event::Event::Type::CollisionImpulse,
//      event::EventManager::Callback{[this](afk::event::Event event) {
//        this->physics_body_system.resolve_collision_event(
//            std::get<afk::event::Event::CollisionImpulse>(event.data));
//      }});

  auto zero_transform        = Transform{};
  zero_transform.translation = glm::vec3{0.0f};

  auto box_entity           = registry.create();
  auto box_transform        = afk::physics::Transform{};
  box_transform.translation = glm::vec3(2.0f, 0.0f, -0.4f);
  box_transform.scale       = glm::vec3(1.0f);
  auto box_model            = afk::render::Model("res/model/box/box.obj");
  //  this->renderer.load_model(box_model);
  //  registry.emplace<afk::physics::Transform>(box_entity, box_transform);
  auto collision_body = afk::physics::CollisionBodyCollection{};
  collision_body.push_back(afk::physics::CollisionBody{
      afk::physics::CollisionBodyType::Box,
      afk::physics::shape::Box{0.1f, 0.3f, 0.1f}, zero_transform});
  collision_body.push_back(afk::physics::CollisionBody{
      afk::physics::CollisionBodyType::Box,
      afk::physics::shape::Box{0.3f, 0.1f, 0.1f}, zero_transform});
  auto physics_body =
      afk::physics::PhysicsBody{box_entity, &this->physics_body_system, box_transform,
                                collision_body, afk::physics::BodyType::Static};
  registry.emplace<afk::physics::PhysicsBody>(box_entity, physics_body);

  auto ball_entity           = registry.create();
  auto ball_transform        = afk::physics::Transform{};
  ball_transform.translation = glm::vec3(1.0f, 0.0f, 1.0f);
  ball_transform.scale       = glm::vec3(1.0f);
  auto collision_body_ball   = afk::physics::CollisionBodyCollection{};
  collision_body_ball.push_back(afk::physics::CollisionBody{
      afk::physics::CollisionBodyType::Sphere, afk::physics::shape::Sphere{2.5f}, zero_transform});
  auto physics_body_ball =
      afk::physics::PhysicsBody{ball_entity, &this->physics_body_system, ball_transform,
                                collision_body_ball, afk::physics::BodyType::Dynamic};
  registry.emplace<afk::physics::PhysicsBody>(ball_entity, physics_body_ball);
  registry.emplace<afk::physics::Transform>(ball_entity, ball_transform);

  auto slab_entity           = registry.create();
  auto slab_transform        = afk::physics::Transform{};
  slab_transform.translation = glm::vec3(0.0f, -30.0f, 1.0f);
  slab_transform.scale       = glm::vec3(1.0f);
  auto slab_collision_body   = afk::physics::CollisionBodyCollection{};
  slab_collision_body.push_back(afk::physics::CollisionBody{
      afk::physics::CollisionBodyType::Box,
      afk::physics::shape::Box{5.0f, 0.1f, 5.0f}, Transform{}});
  auto slab_physics_body =
      afk::physics::PhysicsBody{slab_entity, &this->physics_body_system, slab_transform,
                                slab_collision_body, afk::physics::BodyType::Static};
  registry.emplace<afk::physics::PhysicsBody>(slab_entity, slab_physics_body);
  registry.emplace<afk::physics::Transform>(slab_entity, slab_transform);

  this->is_initialized = true;
}

auto Engine::render() -> void {
  //  auto t        = Transform{};
  //  t.scale       = vec3{0.25f};
  //  t.translation = vec3{0.0f, -1.0f, 0.0f};

  //  this->renderer.queue_draw({"res/model/city/city.fbx", "shader/default.prog", t});

  //  auto t2 = Transform{};
  //  t2.translation = vec3{-1.0f, 0.0f, 2.0f};
  //  this->renderer.queue_draw({"res/model/box/box.obj", "shader/default.prog", t2});
  //  auto t3 = Transform{};
  //  t3.translation = vec3{1.0f, 0.0f, 2.0f};
  //  this->renderer.queue_draw({"res/model/box/box.obj", "shader/default.prog", t3});

  auto t4           = Transform{};
  t4.translation    = vec3{0.0f};
  auto render_model = this->physics_body_system.get_debug_model();
  if (!render_model.meshes[0].vertices.empty()) {
    auto render_model_handle = this->renderer.load_model(render_model);
    this->renderer.queue_draw({render_model.file_path, "shader/default.prog", t4});
  }

  this->renderer.clear_screen({135.0f, 206.0f, 235.0f, 1.0f});
  this->ui_manager.prepare();
  this->renderer.draw();
  this->event_manager.pump_render();
  this->ui_manager.draw();
  this->renderer.swap_buffers();
}

auto Engine::update() -> void {
  this->event_manager.pump_events();

  this->update_camera_pos();

  if (glfwWindowShouldClose(this->renderer.window)) {
    this->is_running = false;
  }

  if (this->ui_manager.show_menu) {
    glfwSetInputMode(this->renderer.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  } else {
    glfwSetInputMode(this->renderer.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }

  this->physics_body_system.update(afk::Engine::get_time());

  // this->update_camera();

  ++this->frame_count;
  this->last_update = afk::Engine::get_time();
}

auto Engine::exit() -> void {
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

auto Engine::update_camera_pos() -> void {
  if (!this->ui_manager.show_menu) {
    if (this->event_manager.key_state.at(Event::Action::Forward)) {
      this->camera.handle_key(Movement::Forward, this->get_delta_time());
    }

    if (this->event_manager.key_state.at(Event::Action::Left)) {
      this->camera.handle_key(Movement::Left, this->get_delta_time());
    }

    if (this->event_manager.key_state.at(Event::Action::Backward)) {
      this->camera.handle_key(Movement::Backward, this->get_delta_time());
    }

    if (this->event_manager.key_state.at(Event::Action::Right)) {
      this->camera.handle_key(Movement::Right, this->get_delta_time());
    }
  }
}
