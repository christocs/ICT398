#pragma once

#include <string>
#include <variant>

#include "afk/NumericTypes.hpp"

namespace afk {
  namespace event {
    /**
     * Encapsulates an engine event.
     */
    struct Event {
      /**
       * Encapsulates a mouse move event.
       */
      struct MouseMove {
        /** The x position of the cursor. */
        f64 x = {};
        /** The y position of the cursor. */
        f64 y = {};
      };

      /**
       * Encapsulates a mouse button event.
       */
      struct MouseButton {
        /** The mouse button clicked. */
        i32 button = {};
        /** Was the control key held? */
        bool control = {};
        /** Was the alt key held? */
        bool alt = {};
        /** Was the shift key held? */
        bool shift = {};
      };

      /**
       * Encapsulates a key press event.
       */
      struct Key {
        /** The key pressed. */
        i32 key = {};
        /** The scancode of the key pressed. */
        i32 scancode = {};
        /** Was the control key held? */
        bool control = {};
        /** Was the alt key held? */
        bool alt = {};
        /** Was the shift key held? */
        bool shift = {};
      };

      /**
       * Encapsulates a text input event.
       */
      struct Text {
        /** The Unicode codepoint of the text entered. */
        u32 codepoint = {};
      };

      /**
       * Encapsulates a mouse scroll event.
       */
      struct MouseScroll {
        /** The x axis of the scroll offset. */
        f64 x = {};
        /** The y axis of the scroll offset. */
        f64 y = {};
      };

      /**
       * Encapsulates a physics update event.
       */
      struct Update {
        /** The delta time of the last frame. */
        f32 dt = {};
      };

      struct Render {};

      /**
       * Denotes a movement action.
       * @todo Move to input manager.
       */
      enum class Action { Forward, Backward, Left, Right };

      /**
       * Denotes an event type.
       */
      enum class Type {
        MouseDown,
        MouseUp,
        MouseMove,
        KeyDown,
        KeyUp,
        KeyRepeat,
        TextEnter,
        MouseScroll,
        Update,
        Render,
      };

      /**
       * Encapsulates all possible event data.
       */
      using Data =
          std::variant<std::monostate, MouseMove, MouseButton, Key, Text, MouseScroll, Update, Render>;

      /**
       * Returns the data contained in this event.
       *
       * @return The data contained in this event.
       */
      template<typename T>
      auto get() -> T {
        return std::get<T>(this->data);
      }

      /** The event data. */
      Data data = {};
      /** The event type. */
      Type type = {};
    };
  }
}
