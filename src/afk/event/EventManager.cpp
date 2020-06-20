#include "afk/event/EventManager.hpp"

#include "afk/Afk.hpp"
#include "afk/debug/Assert.hpp"
#include "afk/event/Event.hpp"
#include "afk/io/Log.hpp"

// Must be included after GLAD.
#include <algorithm>

#include <GLFW/glfw3.h>

using Afk::EventManager;
using Action = Afk::Event::Action;

std::size_t EventManager::Callback::index = 0;
EventManager::Callback::Callback(std::function<void(Afk::Event)> fn)
  : func(fn), id(index++) {}
auto EventManager::Callback::operator==(const EventManager::Callback &rhs) const -> bool {
  return this->id == rhs.id;
}
auto EventManager::Callback::operator()(const Afk::Event &arg) const -> void {
  this->func(arg);
}

auto EventManager::initialize(Renderer::Window window) -> void {
  afk_assert(!this->is_initialized, "Event manager already initialized");
  this->setup_callbacks(window);
  this->is_initialized = true;
}
auto EventManager::pump_render() -> void {
  constexpr auto render_event = Event{Event::Render{}, Event::Type::Render};
  for (const auto &event_callback : this->callbacks[Event::Type::Render]) {
    event_callback(render_event);
  }
}

auto EventManager::pump_events() -> void {
  glfwPollEvents();
  this->events.push({Event::Update{Afk::Engine::get().get_delta_time()}, Event::Type::Update});

  while (this->events.size() > 0) {
    const auto &current_event = this->events.front();

    for (const auto &event_callback : this->callbacks[current_event.type]) {
      event_callback(current_event);
    }

    this->events.pop();
  }
}

auto EventManager::queue_event(Event event) -> void {
  this->events.push(event);
}

auto EventManager::register_event(Event::Type type, Callback callback) -> void {
  this->callbacks[type].push_back(callback);
}
auto EventManager::deregister_event(Event::Type type, Callback callback) -> void {
  auto &typ_callbacks = this->callbacks[type];
  auto callback_pos = std::find(typ_callbacks.begin(), typ_callbacks.end(), callback);
  typ_callbacks.erase(callback_pos);
}

auto EventManager::setup_callbacks(Renderer::Window window) -> void {
  glfwSetMouseButtonCallback(window, EventManager::mouse_press_callback);
  glfwSetScrollCallback(window, EventManager::mouse_scroll_callback);
  glfwSetKeyCallback(window, EventManager::key_callback);
  glfwSetCharCallback(window, EventManager::char_callback);
  glfwSetCursorPosCallback(window, EventManager::mouse_pos_callback);
  glfwSetErrorCallback(EventManager::error_callback);
}

auto EventManager::key_callback([[maybe_unused]] GLFWwindow *window, int key,
                                int scancode, int action, int mods) -> void {
  auto &afk = Engine::get();

  const auto control = (mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL;
  const auto alt     = (mods & GLFW_MOD_ALT) == GLFW_MOD_ALT;
  const auto shift   = (mods & GLFW_MOD_SHIFT) == GLFW_MOD_SHIFT;

  auto type = Event::Type{};

  switch (action) {
    case GLFW_PRESS: type = Event::Type::KeyDown; break;
    case GLFW_RELEASE: type = Event::Type::KeyUp; break;
    case GLFW_REPEAT: type = Event::Type::KeyRepeat; break;
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

auto EventManager::char_callback([[maybe_unused]] GLFWwindow *window, uint32_t codepoint)
    -> void {
  Afk::Engine::get().event_manager.events.push({Event::Text{codepoint}, Event::Type::TextEnter});
}

auto EventManager::mouse_pos_callback([[maybe_unused]] GLFWwindow *window,
                                      double x, double y) -> void {
  Afk::Engine::get().event_manager.events.push({Event::MouseMove{x, y}, Event::Type::MouseMove});
}

auto EventManager::mouse_press_callback([[maybe_unused]] GLFWwindow *window, int button,
                                        int action, [[maybe_unused]] int mods) -> void {
  auto &afk = Engine::get();

  const auto control = (mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL;
  const auto alt     = (mods & GLFW_MOD_ALT) == GLFW_MOD_ALT;
  const auto shift   = (mods & GLFW_MOD_SHIFT) == GLFW_MOD_SHIFT;
  const auto type = action == GLFW_PRESS ? Event::Type::MouseDown : Event::Type::MouseUp;

  afk.event_manager.events.push({Event::MouseButton{button, control, alt, shift}, type});
}

auto EventManager::mouse_scroll_callback([[maybe_unused]] GLFWwindow *window,
                                         double dx, double dy) -> void {
  Afk::Engine::get().event_manager.events.push(
      {Event::MouseScroll{dx, dy}, Event::Type::MouseScroll});
}

auto EventManager::error_callback([[maybe_unused]] int error, const char *msg) -> void {
  std::cerr << "glfw error: " << msg << '\n';
}
