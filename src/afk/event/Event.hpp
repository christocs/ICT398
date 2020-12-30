#pragma once

#include <string>
#include <variant>

#include <glm/glm.hpp>
#include "afk/NumericTypes.hpp"
#include "afk/ecs/Entity.hpp"

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
       * Encapsulates a collision event.
       */
      struct Collision {
        /** Representation of a single contact in a collision */
        struct Contact {
          /** Point of contact on collider 1 in world space */
          glm::vec3 collider1_point;
          /** Point of contact on collider 2 in world space */
          glm::vec3 collider2_point;
          /** normal of contact from the first to second body relative to the world space */
          glm::vec3 normal;
          /** get penetration depth of the collision */
          f32 penetration_depth;
        };

        using ContactCollection = std::vector<Contact>;

        /** The first entity in the collision */
        ecs::Entity entity1;
        /** The second entity in the collision */
        ecs::Entity entity2;
        /** The collection of collision contacts */
        ContactCollection contacts = {};
      };

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
        Collision,
      };

      /**
       * Encapsulates all possible event data.
       */
      using Data = std::variant<std::monostate, MouseMove, MouseButton, Key, Text, MouseScroll, Collision>;

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
