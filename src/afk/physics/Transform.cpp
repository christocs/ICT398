#include "afk/physics/Transform.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>

using afk::physics::Transform;
using glm::mat4;

Transform::Transform(GameObject e) {
  this->owning_entity = e;
}

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

Transform::Transform(GameObject e, mat4 transform) {
  this->owning_entity = e;

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
