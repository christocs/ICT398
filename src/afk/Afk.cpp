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

/**
 * Initializes the afk engine.
 */
auto Engine::initialize() -> void {
  afk_assert(!this->is_initialized, "Engine already initialized");

  this->renderer.initialize();
  this->event_manager.initialize(this->renderer.window);
  this->ui_manager.initialize(this->renderer.window);

  this->event_manager.register_event(Event::Type::MouseMove,
                                     event::eventManager::Callback{[this](Event event) {
                                       this->move_mouse(event);
                                     }});
  this->event_manager.register_event(
      Event::Type::KeyDown, event::eventManager::Callback{[this](Event event) {
        this->move_keyboard(event);
      }});

  this->is_initialized = true;
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

/**
 * Returns a reference to the current engine instance.
 *
 * @return Returns a reference to the current engine instance.
 */
auto Engine::get() -> Engine & {
  static auto instance = Engine{};

  return instance;
}

/**
 * Exits the engine.
 */
auto Engine::exit() -> void {
  this->is_running = false;
}

/**
 * Draws one frame and swaps the front and back framebuffer.
 */
auto Engine::render() -> void {
  auto t        = Transform{};
  t.scale       = vec3{0.25f};
  t.translation = vec3{0.0f, -1.0f, 0.0f};

  this->renderer.queue_draw({"res/model/city/city.fbx", "shader/default.prog", t});

  this->renderer.clear_screen({135.0f, 206.0f, 235.0f, 1.0f});
  this->ui_manager.prepare();
  this->renderer.draw();
  this->event_manager.pump_render();
  this->ui_manager.draw();
  this->renderer.swap_buffers();
}

/**
 * Advances the game simulation for one tick.
 */
auto Engine::update() -> void {
  this->event_manager.pump_events();

  if (glfwWindowShouldClose(this->renderer.window)) {
    this->is_running = false;
  }

  if (this->ui_manager.show_menu) {
    glfwSetInputMode(this->renderer.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  } else {
    glfwSetInputMode(this->renderer.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }

  // this->update_camera();

  ++this->frame_count;
  this->last_update = afk::Engine::get_time();
}

/**
 * Returns the current time in seconds.
 *
 * The current time is counted since the start of the engine.
 *
 * @return Returns the current time in seconds.
 */
auto Engine::get_time() -> f32 {
  return static_cast<f32>(glfwGetTime());
}

/**
 * Returns the delta time of the last update in seconds.
 *
 * @return Returns the delta time of the last update in seconds.
 */
auto Engine::get_delta_time() -> f32 {
  return this->get_time() - this->last_update;
}

/**
 * Returns if the engine is running.
 *
 * @return Returns if the engine is running.
 */
auto Engine::get_is_running() const -> bool {
  return this->is_running;
}
