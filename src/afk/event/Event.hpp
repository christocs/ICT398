#pragma once

#include <string>
#include <variant>
#include <glm/glm.hpp>

#include "afk/NumericTypes.hpp"
#include "afk/physics/PhysicsBody.hpp"

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

      // todo: better names
      struct CollisionImpulseBodyData {
        CollisionImpulseBodyData(afk::physics::BodyType _type,
                                 afk::GameObject _id, glm::vec3 _contact_point)
          : type(_type), id(_id), contact_point(_contact_point) {}
        CollisionImpulseBodyData() = delete;

        afk::physics::BodyType type; // type can be retrieved from physics body component, but stored here for easy access
        afk::GameObject id;
        glm::vec3 contact_point = {};
      };

      struct CollisionImpulse {
        CollisionImpulseBodyData body1;
        CollisionImpulseBodyData body2;
        std::vector<glm::vec3> collision_normals = {};
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
        Update,
        Render,
        CollisionImpulse, // todo: better name
      };

      /**
       * Encapsulates all possible event data.
       */
      using Data =
          std::variant<std::monostate, MouseMove, MouseButton, Key, Text, MouseScroll, Update, Render, CollisionImpulse>;

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
