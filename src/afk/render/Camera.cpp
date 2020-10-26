#include "afk/render/Camera.hpp"

#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "afk/NumericTypes.hpp"

using glm::mat4;
using glm::vec2;
using glm::vec3;

using afk::render::Camera;

auto Camera::handle_mouse(f32 dx, f32 dy) -> void {
  constexpr auto max_yaw = 89.0f;

  this->angles.x += dx * this->sensitivity;
  this->angles.y += -dy * this->sensitivity;
  this->angles.y = std::clamp(this->angles.y, -max_yaw, max_yaw);
}

auto Camera::handle_key(Movement movement, f32 dt) -> void {
  const auto velocity = this->speed * dt;

  switch (movement) {
    case Movement::Forward: {
      this->position += this->get_front() * velocity;
    } break;
    case Movement::Backward: {
      this->position -= this->get_front() * velocity;
    } break;
    case Movement::Left: {
      this->position -= this->get_right() * velocity;
    } break;
    case Movement::Right: {
      this->position += this->get_right() * velocity;
    } break;
  }
}

auto Camera::get_view_matrix() const -> mat4 {
  return glm::lookAt(this->position, this->position + this->get_front(), this->get_up());
}

auto Camera::get_projection_matrix(int width, i32 height) const -> mat4 {
  const auto w      = static_cast<f32>(width);
  const auto h      = static_cast<f32>(height);
  const auto aspect = h > 0 ? w / h : 0;

  return glm::perspective(glm::radians(this->fov), aspect, this->near, this->far);
}

auto Camera::get_front() const -> vec3 {
  auto front = vec3{};

  front.x = std::cos(glm::radians(this->angles.x)) *
            std::cos(glm::radians(this->angles.y));
  front.y = std::sin(glm::radians(this->angles.y));
  front.z = std::sin(glm::radians(this->angles.x)) *
            std::cos(glm::radians(this->angles.y));

  front = glm::normalize(front);

  return front;
}

auto Camera::get_right() const -> vec3 {
  return glm::normalize(glm::cross(this->get_front(), this->WORLD_UP));
}

auto Camera::get_up() const -> vec3 {
  return glm::normalize(glm::cross(this->get_right(), this->get_front()));
}

auto Camera::get_position() const -> vec3 {
  return this->position;
}

auto Camera::get_angles() const -> vec2 {
  return this->angles;
}

auto Camera::set_position(glm::vec3 v) -> void {
  this->position = v;
}
auto Camera::set_angles(glm::vec2 v) -> void {
  this->angles = v;
}

auto Camera::get_key(Movement movement) -> bool {
  return this->key_states.at(movement);
}

auto Camera::set_key(Movement movement, bool down) -> void {
  this->key_states.at(movement) = down;
}
