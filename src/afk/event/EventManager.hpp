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
    /**
     * Manages events within the engines. Allows registering callbacks to
     * to specific events.
     */
    class EventManager {
    public:
      /**
       * Wraps a std::function callback in order to make them comparable.
       */
      class Callback {
      public:
        /** The underlying function type. */
        using Function = std::function<void(Event)>;

        /**
         * Constructs a new callback from the specified function.
         *
         * @param _fn The function to wrap.
         */
        Callback(Function _fn);

        /**
         * Returns if the right hand side callback is the same as this callback.
         *
         * @param rhs The right hand side callback.
         * @return True if the right hand side callback is the same as this callback.
         */
        auto operator==(const Callback &rhs) const -> bool;

        /**
         * Calls this callback with the specified event argument.
         *
         * @param arg The event argument to forward.
         */
        auto operator()(const Event &arg) const -> void;

      private:
        /** The function being wrapped. */
        Function fn = {};
        /** The callback ID. */
        usize id = {};
        /** The next callback ID. */
        static usize next_id;
      };

      EventManager()                     = default;
      EventManager(EventManager &&)      = delete;
      EventManager(const EventManager &) = delete;
      auto operator=(const EventManager &) -> EventManager & = delete;
      auto operator=(EventManager &&) -> EventManager & = delete;

      /**
       * Initializes this event manager.
       *
       * @param window The window to hook into.
       */
      auto initialize(render::Renderer::Window window) -> void;

      /**
       * Pumps and calls all render event callbacks.
       */
      auto pump_render() -> void;

      /**
       * Pumps and calls all event callbacks.
       */
      auto pump_events() -> void;

      /**
       * Pushes an event to the event queue.
       *
       * @param event The event to push.
       */
      auto push_event(Event event) -> void;

      /**
       * Registers the specified callback to the specified event type.
       *
       * @param type The event type.
       * @param callback The callback to register.
       */
      auto register_event(Event::Type type, Callback callback) -> void;

      /**
       * Deregisters the specified callback from the specified event type.
       *
       * @param type The event type.
       * @param callback The callback to deregister.
       */
      auto deregister_event(Event::Type type, Callback callback) -> void;

      /**
       * Sets up the underlying GLFW callbacks.
       *
       * @param window The current window.
       */
      auto setup_callbacks(render::Renderer::Window window) -> void;

      /**
       * Stores the current key state.
       * @todo Move to input manager.
       */
      std::unordered_map<Event::Action, bool> key_state = {
          {Event::Action::Forward, false},
          {Event::Action::Left, false},
          {Event::Action::Backward, false},
          {Event::Action::Right, false},
      };

    private:
      /**
       * Forwards GLFW key events to the engine.
       *
       * @param window The GLFW window.
       * @param key The GLFW key.
       * @param scancode The GLFW scancode.
       * @param action The GLFW action.
       * @param mods The GLFW modifiers.
       */
      static auto key_callback(GLFWwindow *window, i32 key, i32 scancode,
                               i32 action, i32 mods) -> void;

      /**
       * Forwards GLFW character events to the engine.
       *
       * @param window The GLFW window.
       * @param codepoint The unicode codepoint.
       */
      static auto char_callback(GLFWwindow *window, u32 codepoint) -> void;
      /**
       * Forwards GLFW mouse position events to the engine.
       *
       * @param window The GLFW window.
       * @param x The GLFW mouse x position.
       * @param y The GLFW mouse y position.
       */
      static auto mouse_pos_callback(GLFWwindow *window, f64 x, f64 y) -> void;

      /**
       * Forwards GLFW mouse press events to the engine.
       *
       * @param window The GLFW window.
       * @param button The GLFW button.
       * @param action The GLFW action.
       * @param mods The GLFW modifiers.
       */
      static auto mouse_press_callback(GLFWwindow *window, i32 button,
                                       i32 action, i32 mods) -> void;

      /**
       * Forwards GLFW mouse scroll events to the engine.
       *
       * @param window The GLFW window.
       * @param dx The GLFW scrollwheel delta x.
       * @param dy The GLFW scrollwheel delta y.
       */
      static auto mouse_scroll_callback(GLFWwindow *window, f64 dx, f64 dy) -> void;

      /**
       * Reports GLFW error events.
       *
       * @param error The error code.
       * @param msg The error message.
       */
      static auto error_callback(i32 error, const char *msg) -> void;

      /** Is the event manager initialized? */
      bool is_initialized = false;
      /** The event queue. */
      std::queue<Event> events = {};

      /** Maps each event type to a collection of callbacks. */
      std::unordered_map<Event::Type, std::vector<Callback>> callbacks = {
          {Event::Type::MouseDown, {}},   {Event::Type::MouseUp, {}},
          {Event::Type::MouseMove, {}},   {Event::Type::KeyDown, {}},
          {Event::Type::KeyUp, {}},       {Event::Type::TextEnter, {}},
          {Event::Type::MouseScroll, {}}, {Event::Type::Update, {}},
      };
    };
  }
}
