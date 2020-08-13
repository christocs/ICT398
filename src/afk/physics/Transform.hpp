#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "afk/component/BaseComponent.hpp"

namespace afk {
  namespace physics {
    /**
     * Encapsulates the transformation of an object in 3D space.
     */
    struct Transform {
      /** The objects translation. */
      glm::vec3 translation = glm::vec3{1.0f};
      /** The objects scale. */
      glm::vec3 scale = glm::vec3{1.0f};
      /** The objects rotation as a quaternion. */
      glm::quat rotation = glm::quat{1.0f, 0.0f, 0.0f, 0.0f};

      Transform() = default;

      /**
       * Constructs a new transform from the specified 4x4 transformation matrix.
       *
       * @param transform The transformation matrix.
       */
      Transform(glm::mat4 transform);
    };
  }
}
