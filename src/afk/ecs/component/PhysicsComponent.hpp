#pragma once

#include <variant>

#include <glm/glm.hpp>

#include "afk/NumericTypes.hpp"

namespace afk {
  namespace ecs {
    namespace component {
      /**
       * Encapsulates a physics component.
       *
       * Any entity with a ColliderComponent but without a PhysicsComponent is assumed to be a static rigid body
       * Any entity with a ColliderComponent and a PhysicsComponent is assumed to be a dynamic rigid body
       */
      struct PhysicsComponent {
        /** --- more general data --- */

        /** if the component is static */
        bool is_static = true;
        /** center of mass local to the entity */
        glm::vec3 center_of_mass = glm::vec3{};
        /** mass in kilograms */
        f32 total_mass = {};
        /** inverse of mass */
        f32 total_inverse_mass = {};

        /** --- linear data --- */

        /** linear velocity */
        glm::vec3 linear_velocity = glm::vec3{0.0f};
        /** external force to be applied then reset on the next update */
        glm::vec3 external_forces = glm::vec3{0.0f};
        /** linear dampening */
        f32 linear_dampening = 1.0f;

        /** --- angular data --- */

        /** angular velocity */
        glm::vec3 angular_velocity = glm::vec3{0.0f};
        /** external torque to be applied then reset on the next update */
        glm::vec3 external_torques = glm::vec3{0.0f};
        /** angular dampening */
        f32 angular_dampening = 1.0f;
        /** inertial tensor local to the rigid body */
        glm::mat3x3 inertial_tensor = glm::mat3x3{1.0f};
        /** inverse inertial tensor local to the rigid body */
        glm::mat3x3 inverse_inertial_tensor = glm::mat3x3{1.0f};
      };
    }
  }
}
