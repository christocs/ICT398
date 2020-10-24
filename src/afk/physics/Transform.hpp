#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace afk {
  namespace physics {
    /**
     * Encapsulates the transformation of an object in 3D space.
     */
    struct Transform {
      /** The object's translation. */
      glm::vec3 translation = glm::vec3{};
      /** The object's scale. */
      glm::vec3 scale = glm::vec3{1.0f};
      /** The object's rotation as a quaternion. */
      glm::quat rotation = glm::quat{};

      Transform() = default;

      /**
       * Constructs a new transform from the specified 4x4 transformation matrix.
       *
       * @param transform The transformation matrix.
       */
      Transform(glm::mat4 transform);

      /**
       * Return the transform as a 4x4 transformation matrix
       * 
       * @return transformation matrix
       */
      auto to_mat4() -> glm::mat4;
    };
  }
}
