#include "Events.hpp"

#include <GLFW/glfw3.h>

#include "afk/Afk.hpp"
using Afk::EventManager;

EventManager::EventManager() {}

auto EventManager::pump_events() -> void {
  glfwPollEvents();
  this->events.push({Event::Update{Afk::Engine::get().get_delta_time()},
                     Event::EventType::Update});
  while (this->events.size() > 0) {
    const auto &current_event = this->events.front();
    for (auto &event_callback : this->callbacks[current_event.type]) {
      event_callback(current_event);
    }
    this->events.pop();
  }
}
auto EventManager::register_event(Event::EventType type, Callback callback) -> void {
  this->callbacks[type].push_back(callback);
}

auto EventManager::setup_callbacks(GLFWwindow *win) -> void {
  glfwSetKeyCallback(win, key_callback);
  glfwSetCharCallback(win, char_callback);
  glfwSetCursorPosCallback(win, mouse_pos_callback);
  glfwSetMouseButtonCallback(win, mouse_press_callback);
  glfwSetScrollCallback(win, mouse_scroll_callback);
}

auto EventManager::key_callback(GLFWwindow *win, int key, int scancode,
                                int action, int mods) -> void {
  if (action == GLFW_REPEAT) {
    return;
  }
  auto control = (mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL;
  auto alt     = (mods & GLFW_MOD_ALT) == GLFW_MOD_ALT;
  auto shift   = (mods & GLFW_MOD_SHIFT) == GLFW_MOD_SHIFT;
  Afk::Engine::get().events.events.push(
      {Event::Key{
           key,
       },
       action == GLFW_PRESS ? Event::EventType::KeyDown : Event::EventType::KeyUp});
}
auto EventManager::char_callback(GLFWwindow *win, uint32_t codepoint) -> void {
  throw "not implemented";
  Afk::Engine::get().events.events.push(
      {Event::Text{std::string{""}}, Event::EventType::TextEnter});
}
auto EventManager::mouse_pos_callback(GLFWwindow *win, double xpos, double ypos) -> void {
  Afk::Engine::get().events.events.push(
      {Event::MouseMove{xpos, ypos}, Event::EventType::MouseMove});
}
auto EventManager::mouse_press_callback(GLFWwindow *win, int btn, int action, int mods)
    -> void {
  Afk::Engine::get().events.events.push(
      {Event::MouseButton{btn}, action == GLFW_PRESS ? Event::EventType::MouseDown
                                                     : Event::EventType::MouseUp});
}
auto EventManager::mouse_scroll_callback(GLFWwindow *win, double xdelta, double ydelta)
    -> void {
  Afk::Engine::get().events.events.push(
      {Event::MouseScroll{xdelta, ydelta}, Event::EventType::MouseScroll});
}
