#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "afk/component/BaseComponent.hpp"

namespace Afk {
  /**
   * Entity transform component
   */
  struct Transform : public BaseComponent {
    glm::vec3 translation = glm::vec3{1.0f};
    glm::vec3 scale       = glm::vec3{1.0f};
    glm::quat rotation    = glm::quat{1.0f, 0.0f, 0.0f, 0.0f};

    Transform() = default;
    Transform(GameObject e);
    Transform(glm::mat4 transform);
    Transform(GameObject e, glm::mat4 transform);
  };
}
