#pragma once

#include <functional>
#include <queue>
#include <unordered_map>
#include <vector>

#include "afk/NumericTypes.hpp"
#include "afk/event/Event.hpp"
#include "afk/render/Renderer.hpp"

struct GLFWwindow;

namespace afk {
  namespace event {
    class eventManager {
    public:
      /**
       * wraps an std::function to make them comparable
       */
      class Callback {
      public:
        Callback(std::function<void(Event)>);
        auto operator==(const Callback &rhs) const -> bool;
        auto operator()(const Event &arg) const -> void;

      private:
        std::function<void(Event)> func;
        usize id;
        static usize index;
      };

      eventManager()                     = default;
      eventManager(eventManager &&)      = delete;
      eventManager(const eventManager &) = delete;
      auto operator=(const eventManager &) -> eventManager & = delete;
      auto operator=(eventManager &&) -> eventManager & = delete;
      /**
       * Init event manager
       */
      auto initialize(render::renderer::Window window) -> void;
      /**
       * pump render events
       */
      auto pump_render() -> void;
      /**
       * pump global events (update, keypress, etc)
       */
      auto pump_events() -> void;
      /**
       * queue event
       */
      auto push_event(Event event) -> void;
      /**
       * register an event
       */
      auto register_event(Event::Type type, Callback callback) -> void;
      /**
       * deregister an event
       */
      auto deregister_event(Event::Type type, Callback callback) -> void;
      /**
       * setup global callbacks from the renderer system
       */
      auto setup_callbacks(render::renderer::Window window) -> void;

      // FIXME: Move to keyboard manager.
      std::unordered_map<Event::Action, bool> key_state = {
          {Event::Action::Forward, false},
          {Event::Action::Left, false},
          {Event::Action::Backward, false},
          {Event::Action::Right, false},
      };

    private:
      static auto key_callback(GLFWwindow *window, i32 key, i32 scancode,
                               i32 action, i32 mods) -> void;
      static auto char_callback(GLFWwindow *window, u32 codepoint) -> void;
      static auto mouse_pos_callback(GLFWwindow *window, f64 x, f64 y) -> void;
      static auto mouse_press_callback(GLFWwindow *window, i32 button,
                                       i32 action, i32 mods) -> void;
      static auto mouse_scroll_callback(GLFWwindow *window, f64 dx, f64 dy) -> void;
      static auto error_callback(i32 error, const char *msg) -> void;

      bool is_initialized      = false;
      std::queue<Event> events = {};

      std::unordered_map<Event::Type, std::vector<Callback>> callbacks = {
          {Event::Type::MouseDown, {}},   {Event::Type::MouseUp, {}},
          {Event::Type::MouseMove, {}},   {Event::Type::KeyDown, {}},
          {Event::Type::KeyUp, {}},       {Event::Type::TextEnter, {}},
          {Event::Type::MouseScroll, {}}, {Event::Type::Update, {}},
          {Event::Type::Collision, {}}};
    };

  }
}
