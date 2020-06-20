#pragma once

#include <glm/glm.hpp>

namespace Afk {
  class Camera {
  public:
    static constexpr glm::vec3 WORLD_UP = {0.0f, 1.0f, 0.0f};
    /**
     * Deprecated
     * \todo Remove
     */
    enum class Movement { Forward, Backward, Left, Right };
    /**
     * Deprecated
     * \todo Remove
     */
    auto handle_mouse(float deltaX, float deltaY) -> void;
    /**
     * Deprecated
     * \todo Remove
     */
    auto handle_key(Movement movement, float deltaTime) -> void;

    /**
     * Get the current view matrix
     */
    auto get_view_matrix() const -> glm::mat4;
    /**
     * Get projection matrix
     */
    auto get_projection_matrix(int width, int height) const -> glm::mat4;
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
    float fov         = 75.0f;
    float near        = 0.1f;
    float far         = 1000.0f;
    float speed       = 10.0f;
    float sensitivity = 0.1f;

    glm::vec2 angles   = {};
    glm::vec3 position = {};
  };
}
