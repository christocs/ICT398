#include "afk/event/EventManager.hpp"

#include "afk/Engine.hpp"
#include "afk/NumericTypes.hpp"
#include "afk/debug/Assert.hpp"
#include "afk/event/Event.hpp"
#include "afk/io/Log.hpp"
#include "afk/io/Time.hpp"

// Must be included after GLAD.
#include <algorithm>

#include <GLFW/glfw3.h>

using afk::event::Event;
using afk::event::EventManager;
using WindowHandle = afk::render::Renderer::WindowHandle;
using Type         = afk::event::Event::Type;

EventManager::Callback::Callback(Callback::Function _fn)
  : fn(_fn), id(next_id++) {}

auto EventManager::Callback::operator==(const EventManager::Callback &rhs) const -> bool {
  return this->id == rhs.id;
}

auto EventManager::Callback::operator()(const Event &arg) const -> void {
  this->fn(arg);
}

auto EventManager::initialize(WindowHandle window_handle) -> void {
  afk_assert(!this->is_initialized, "Event manager already initialized");
  this->setup_callbacks(window_handle);
  this->is_initialized = true;
  afk::io::log << afk::io::get_date_time() << "Event subsystem initialized\n";
}

auto EventManager::pump_events() -> void {
  glfwPollEvents();

  while (this->events.size() > 0) {
    const auto &current_event = this->events.front();

    for (const auto &event_callback : this->callbacks[current_event.type]) {
      event_callback(current_event);
    }

    this->events.pop();
  }
}

auto EventManager::push_event(Event event) -> void {
  this->events.push(event);
}

auto EventManager::register_event(Type type, Callback callback) -> void {
  this->callbacks[type].push_back(callback);
}
auto EventManager::deregister_event(Type type, Callback callback) -> void {
  auto &type_callbacks = this->callbacks[type];
  auto callback_pos = std::find(type_callbacks.begin(), type_callbacks.end(), callback);
  type_callbacks.erase(callback_pos);
}

auto EventManager::setup_callbacks(WindowHandle window_handle) -> void {
  if (auto window = window_handle.lock()) {
    afk_assert(window != nullptr, "Window is uninitialized");
    glfwSetMouseButtonCallback(window.get(), EventManager::mouse_press_callback);
    glfwSetScrollCallback(window.get(), EventManager::mouse_scroll_callback);
    glfwSetKeyCallback(window.get(), EventManager::key_callback);
    glfwSetCharCallback(window.get(), EventManager::char_callback);
    glfwSetCursorPosCallback(window.get(), EventManager::mouse_pos_callback);
  } else {
    afk_unreachable();
  }

  glfwSetErrorCallback(EventManager::error_callback);
}

auto EventManager::key_callback([[maybe_unused]] GLFWwindow *window, i32 key,
                                i32 scancode, i32 action, i32 mods) -> void {
  auto &afk = Engine::get();

  const auto control = (mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL;
  const auto alt     = (mods & GLFW_MOD_ALT) == GLFW_MOD_ALT;
  const auto shift   = (mods & GLFW_MOD_SHIFT) == GLFW_MOD_SHIFT;

  auto type = Type{};

  switch (action) {
    case GLFW_PRESS: type = Type::KeyDown; break;
    case GLFW_RELEASE: type = Type::KeyUp; break;
    case GLFW_REPEAT: type = Type::KeyRepeat; break;
  }

  afk.event_manager.events.push({Event::Key{key, scancode, control, alt, shift}, type});

  // FIXME: Move to keyboard manager.
  // if (action != GLFW_REPEAT) {
  //   auto new_state = false;
  //   switch (action) {
  //     case GLFW_PRESS: new_state = true; break;
  //     case GLFW_RELEASE: new_state = false; break;
  //   }

  //   switch (key) {
  //     case GLFW_KEY_W:
  //       afk.event_manager.key_state[Action::Forward] = new_state;
  //       break;
  //     case GLFW_KEY_A:
  //       afk.event_manager.key_state[Action::Left] = new_state;
  //       break;
  //     case GLFW_KEY_S:
  //       afk.event_manager.key_state[Action::Backward] = new_state;
  //       break;
  //     case GLFW_KEY_D:
  //       afk.event_manager.key_state[Action::Right] = new_state;
  //       break;
  //   }
  // }
}

auto EventManager::char_callback([[maybe_unused]] GLFWwindow *window, u32 codepoint) -> void {
  auto &afk = Engine::get();

  afk.event_manager.events.push({Event::Text{codepoint}, Type::TextEnter});
}

auto EventManager::mouse_pos_callback([[maybe_unused]] GLFWwindow *window,
                                      f64 x, f64 y) -> void {
  auto &afk = Engine::get();

  afk.event_manager.events.push({Event::MouseMove{x, y}, Type::MouseMove});
}

auto EventManager::mouse_press_callback([[maybe_unused]] GLFWwindow *window, i32 button,
                                        i32 action, [[maybe_unused]] i32 mods) -> void {
  auto &afk = Engine::get();

  const auto control = (mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL;
  const auto alt     = (mods & GLFW_MOD_ALT) == GLFW_MOD_ALT;
  const auto shift   = (mods & GLFW_MOD_SHIFT) == GLFW_MOD_SHIFT;
  const auto type    = action == GLFW_PRESS ? Type::MouseDown : Type::MouseUp;

  afk.event_manager.events.push({Event::MouseButton{button, control, alt, shift}, type});
}

auto EventManager::mouse_scroll_callback([[maybe_unused]] GLFWwindow *window,
                                         f64 dx, f64 dy) -> void {
  auto &afk = Engine::get();

  afk.event_manager.events.push({Event::MouseScroll{dx, dy}, Type::MouseScroll});
}

auto EventManager::error_callback([[maybe_unused]] i32 error, const char *msg) -> void {
  afk::io::log << afk::io::get_date_time() << "glfw error: " << msg << '\n';
}
