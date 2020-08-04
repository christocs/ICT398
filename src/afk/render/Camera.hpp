#pragma once

#include <glm/glm.hpp>

#include "afk/NumericTypes.hpp"

namespace afk {
  namespace render {
    class Camera {
    public:
      static constexpr glm::vec3 WORLD_UP = {0.0f, 1.0f, 0.0f};
      enum class Movement { Forward, Backward, Left, Right };
      auto handle_mouse(f32 deltaX, f32 deltaY) -> void;
      auto handle_key(Movement movement, f32 deltaTime) -> void;

      /**
       * Get the current view matrix
       */
      auto get_view_matrix() const -> glm::mat4;
      /**
       * Get projection matrix
       */
      auto get_projection_matrix(int width, i32 height) const -> glm::mat4;
      /**
       * Current camera position
       */
      auto get_position() const -> glm::vec3;
      /**
       * Current camera look angle
       */
      auto get_angles() const -> glm::vec2;
      /**
       * Set current camera position
       */
      auto set_position(glm::vec3 v) -> void;
      /**
       * Set current camera angles
       */
      auto set_angles(glm::vec2 v) -> void;
      /**
       * Get camera front facing vector
       */
      auto get_front() const -> glm::vec3;
      /**
       * Get camera right facing vector
       */
      auto get_right() const -> glm::vec3;
      /**
       * Get camera up facing vector
       */
      auto get_up() const -> glm::vec3;

    private:
      f32 fov         = 75.0f;
      f32 near        = 0.1f;
      f32 far         = 1000.0f;
      f32 speed       = 10.0f;
      f32 sensitivity = 0.1f;

      glm::vec2 angles   = {};
      glm::vec3 position = {};
    };
  }
}
