#include "afk/physics/Transform.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>

using afk::physics::Transform;
using glm::mat4;

/// @cond DOXYGEN_IGNORE

Transform::Transform(mat4 transform) {
  auto _scale       = glm::vec3{};
  auto _rotation    = glm::quat{};
  auto _translation = glm::vec3{};
  auto _skew        = glm::vec3{};
  auto _perspective = glm::vec4{};

  glm::decompose(transform, _scale, _rotation, _translation, _skew, _perspective);

  this->translation = _translation;
  this->scale       = _scale;
  this->rotation    = _rotation;
}

auto Transform::to_mat4() -> glm::mat4 {
  auto matrix = mat4{1.0f};

  // Apply transformation
  matrix = glm::translate(matrix, this->translation);
  matrix *= glm::mat4_cast(this->rotation);
  matrix = glm::scale(matrix, this->scale);

  return matrix;
}

/// @endcond
