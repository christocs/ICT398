#include "afk/event/EventManager.hpp"

#include "afk/Afk.hpp"
#include "afk/NumericTypes.hpp"
#include "afk/debug/Assert.hpp"
#include "afk/event/Event.hpp"
#include "afk/io/Log.hpp"

// Must be included after GLAD.
#include <algorithm>

#include <GLFW/glfw3.h>

using afk::event::Event;
using afk::event::eventManager;
using Window = afk::render::renderer::Window;
using Action = afk::event::Event::Action;
using Type   = afk::event::Event::Type;

usize eventManager::Callback::index = 0;
eventManager::Callback::Callback(std::function<void(Event)> fn)
  : func(fn), id(index++) {}
auto eventManager::Callback::operator==(const eventManager::Callback &rhs) const -> bool {
  return this->id == rhs.id;
}
auto eventManager::Callback::operator()(const Event &arg) const -> void {
  this->func(arg);
}

auto eventManager::initialize(Window window) -> void {
  afk_assert(!this->is_initialized, "event manager already initialized");
  this->setup_callbacks(window);
  this->is_initialized = true;
}
auto eventManager::pump_render() -> void {
  const auto render_event = Event{Event::render{}, Type::render};

  for (const auto &event_callback : this->callbacks[Type::render]) {
    event_callback(render_event);
  }
}

auto eventManager::pump_events() -> void {
  auto &afk = Engine::get();

  glfwPollEvents();
  this->events.push({Event::Update{afk.get_delta_time()}, Type::Update});

  while (this->events.size() > 0) {
    const auto &current_event = this->events.front();

    for (const auto &event_callback : this->callbacks[current_event.type]) {
      event_callback(current_event);
    }

    this->events.pop();
  }
}

auto eventManager::push_event(Event event) -> void {
  this->events.push(event);
}

auto eventManager::register_event(Type type, Callback callback) -> void {
  this->callbacks[type].push_back(callback);
}

auto eventManager::deregister_event(Type type, Callback callback) -> void {
  auto &type_callbacks = this->callbacks[type];
  auto callback_pos = std::find(type_callbacks.begin(), type_callbacks.end(), callback);
  type_callbacks.erase(callback_pos);
}

auto eventManager::setup_callbacks(Window window) -> void {
  glfwSetMouseButtonCallback(window, eventManager::mouse_press_callback);
  glfwSetScrollCallback(window, eventManager::mouse_scroll_callback);
  glfwSetKeyCallback(window, eventManager::key_callback);
  glfwSetCharCallback(window, eventManager::char_callback);
  glfwSetCursorPosCallback(window, eventManager::mouse_pos_callback);
  glfwSetErrorCallback(eventManager::error_callback);
}

auto eventManager::key_callback([[maybe_unused]] GLFWwindow *window, i32 key,
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
  if (action != GLFW_REPEAT) {
    auto new_state = false;
    switch (action) {
      case GLFW_PRESS: new_state = true; break;
      case GLFW_RELEASE: new_state = false; break;
    }

    switch (key) {
      case GLFW_KEY_W:
        afk.event_manager.key_state[Action::Forward] = new_state;
        break;
      case GLFW_KEY_A:
        afk.event_manager.key_state[Action::Left] = new_state;
        break;
      case GLFW_KEY_S:
        afk.event_manager.key_state[Action::Backward] = new_state;
        break;
      case GLFW_KEY_D:
        afk.event_manager.key_state[Action::Right] = new_state;
        break;
    }
  }
}

auto eventManager::char_callback([[maybe_unused]] GLFWwindow *window, u32 codepoint) -> void {
  auto &afk = Engine::get();

  afk.event_manager.events.push({Event::Text{codepoint}, Type::TextEnter});
}

auto eventManager::mouse_pos_callback([[maybe_unused]] GLFWwindow *window,
                                      f64 x, f64 y) -> void {
  auto &afk = Engine::get();

  afk.event_manager.events.push({Event::MouseMove{x, y}, Type::MouseMove});
}

auto eventManager::mouse_press_callback([[maybe_unused]] GLFWwindow *window, i32 button,
                                        i32 action, [[maybe_unused]] i32 mods) -> void {
  auto &afk = Engine::get();

  const auto control = (mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL;
  const auto alt     = (mods & GLFW_MOD_ALT) == GLFW_MOD_ALT;
  const auto shift   = (mods & GLFW_MOD_SHIFT) == GLFW_MOD_SHIFT;
  const auto type    = action == GLFW_PRESS ? Type::MouseDown : Type::MouseUp;

  afk.event_manager.events.push({Event::MouseButton{button, control, alt, shift}, type});
}

auto eventManager::mouse_scroll_callback([[maybe_unused]] GLFWwindow *window,
                                         f64 dx, f64 dy) -> void {
  auto &afk = Engine::get();

  afk.event_manager.events.push({Event::MouseScroll{dx, dy}, Type::MouseScroll});
}

auto eventManager::error_callback([[maybe_unused]] i32 error, const char *msg) -> void {
  std::cerr << "glfw error: " << msg << '\n';
}
