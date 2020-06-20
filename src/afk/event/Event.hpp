#pragma once

#include <cstdint>
#include <string>
#include <variant>

namespace Afk {
  /**
   * Various event types.
   */
  struct Event {
    struct MouseMove {
      double x = {};
      double y = {};
    };

    struct MouseButton {
      int button   = {};
      bool control = {};
      bool alt     = {};
      bool shift   = {};
    };

    struct Key {
      int key      = {};
      int scancode = {};
      bool control = {};
      bool alt     = {};
      bool shift   = {};
    };

    struct Text {
      uint32_t codepoint = {};
    };

    struct MouseScroll {
      double x = {};
      double y = {};
    };

    struct Update {
      float dt = {};
    };

    struct Render {};

    // using struct of bool to get around luabind having no enum support
    struct CollisionType {
      bool enemy     = {};
      bool prey      = {};
      bool deathzone = {};
    };
    struct CollisionAction {
      bool contact_start = {};
      bool contact_stay = {};
      bool contact_end = {};
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
      Render,
      Collision
    };

    using Data =
        std::variant<std::monostate, MouseMove, MouseButton, Key, Text, MouseScroll, Update, Render, Collision>;

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
