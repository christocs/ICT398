#pragma once

#include <string>
#include <variant>

#include "afk/NumericTypes.hpp"

namespace afk {
  namespace event {
    struct Event {
      struct MouseMove {
        f64 x = {};
        f64 y = {};
      };

      struct MouseButton {
        i32 button   = {};
        bool control = {};
        bool alt     = {};
        bool shift   = {};
      };

      struct Key {
        i32 key      = {};
        i32 scancode = {};
        bool control = {};
        bool alt     = {};
        bool shift   = {};
      };

      struct Text {
        u32 codepoint = {};
      };

      struct MouseScroll {
        f64 x = {};
        f64 y = {};
      };

      struct Update {
        f32 dt = {};
      };

      struct render {};

      // using struct of bool to get around luabind having no enum support
      struct CollisionType {
        bool enemy     = {};
        bool prey      = {};
        bool deathzone = {};
      };
      struct CollisionAction {
        bool contact_start = {};
        bool contact_stay  = {};
        bool contact_end   = {};
      };

      struct Collision {
        CollisionType type;
        CollisionAction action;
      };

      // FIXME: Move to keyboard  handler.
      enum class Action { Forward, Backward, Left, Right };

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
        render,
        Collision
      };

      using Data =
          std::variant<std::monostate, MouseMove, MouseButton, Key, Text, MouseScroll, Update, render, Collision>;

      /**
       * Unwrap variant sugar
       * \todo put this in bindings instead
       */
      template<typename T>
      auto get() -> T {
        return std::get<T>(this->data);
      }

      Data data = {};
      Type type = {};
    };
  }
}
