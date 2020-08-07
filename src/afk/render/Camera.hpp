#pragma once

#include <glm/glm.hpp>

#include "afk/NumericTypes.hpp"

namespace afk {
  namespace render {
    /**
     * Manages the engine camera.
     *
     * @todo Move out camera movement to input manager
     */
    class Camera {
    public:
      /** The world up direction to use. */
      static constexpr glm::vec3 WORLD_UP = {0.0f, 1.0f, 0.0f};
      /** Represents a camera movement direction. */
      enum class Movement { Forward, Backward, Left, Right };

      /**
       * Handles the mouse being moved.
       *
       * @param dx The delta x.
       * @param dy The delta y.
       */
      auto handle_mouse(f32 dx, f32 dy) -> void;

      /**
       * Handles a key being pressed.
       *
       * @param movement The movement type.
       * @param dt The delta time.
       */
      auto handle_key(Movement movement, f32 dt) -> void;

      /**
       * Returns the view matrix.
       *
       * @return The view matrix.
       */
      auto get_view_matrix() const -> glm::mat4;

      /**
       * Returns the projection matrix from the specified with and height.
       *
       * @param width The screen width.
       * @param height The screen height.
       * @return The calculated projection matrix.
       */
      auto get_projection_matrix(int width, i32 height) const -> glm::mat4;

      /**
       * Returns the camera position.
       *
       * @return The camera position.
       */
      auto get_position() const -> glm::vec3;

      /**
       * Returns the camera look angles.
       *
       * @return The camera look angles.
       */
      auto get_angles() const -> glm::vec2;

      /**
       * Moves the camera to the specified position.
       *
       * @param v The new position.
       */
      auto set_position(glm::vec3 v) -> void;

      /**
       * Points the camera at the specified angles.
       *
       * @param v The new angles.
       */
      auto set_angles(glm::vec2 v) -> void;

      /**
       * Returns the camera front vector.
       *
       * @return The camera front vector.
       */
      auto get_front() const -> glm::vec3;

      /**
       * Returns the camera right vector.
       *
       * @return The camera right vector.
       */
      auto get_right() const -> glm::vec3;

      /**
       * Returns the camera up vector.
       *
       * @return The camera up vector.
       */
      auto get_up() const -> glm::vec3;

    private:
      /** The field of view. */
      f32 fov = 75.0f;
      /** The near clipping plane. */
      f32 near = 0.1f;
      /** The far clipping plane. */
      f32 far = 1000.0f;
      /** The movement speed. */
      f32 speed = 10.0f;
      /** The mouse sensitivity. */
      f32 sensitivity = 0.1f;

      /** The look angles. */
      glm::vec2 angles = {};
      /** The position. */
      glm::vec3 position = {};
    };
  }
}
