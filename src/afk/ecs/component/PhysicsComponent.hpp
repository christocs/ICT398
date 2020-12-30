#pragma once

#include <variant>

#include <glm/glm.hpp>

#include "afk/NumericTypes.hpp"

namespace afk {
  namespace ecs {
    namespace component {
      /**
       * Encapsulates a physics component.
       */
      struct PhysicsComponent {
        /** --- more general data --- */

        /**
         * if the component is static
         * is constant after initialisation
         */
        bool is_static = true;

        /**
         * center of mass local to the entity
         * is constant after initialisation
         */
        glm::vec3 center_of_mass = {};

        /** --- linear data --- */

        /**
         * linear velocity
         * Updated on each cycle
         */
        glm::vec3 linear_velocity = glm::vec3{0.0f};

        /**
         * external force to be applied then reset on the next update
         * temporary accumulator for each cycle that will be reset at the end of each cycle
         */
        glm::vec3 external_forces = glm::vec3{0.0f};

        /**
         * linear dampening
         * constant value that determines how much linear motion should be dampened by, 0 for no dampening, 1 for maximum
         */
        f32 linear_dampening = 1.0f;

        /**
         * mass in kilograms, derived from adding all masses of colliders
         * together is constant after initialisation
         */
        f32 total_mass = {};

        /**
         * inverse of mass, derived from 1 / total_mass
         * is constant after initialisation
         */
        f32 total_inverse_mass = {};

        /** --- angular data --- */

        /**
         * angular velocity
         * updated on eahc update cycle
         */
        glm::vec3 angular_velocity = glm::vec3{0.0f};

        /**
         * external torque to be applied then reset on the next update
         * temporary accumulator for each cycle that will be reset at the end of each cycle
         */
        glm::vec3 external_torques = glm::vec3{0.0f};

        /**
         * angular dampening
         * constant value that determines how much angular motion should be dampened by, 0 for no dampening, 1 for maximum
         */
        f32 angular_dampening = 1.0f;

        /**
         * inertia tensor in world rotation
         * changes on each update depending on the local inertia tensor and the rotation of the entity
         */
        glm::mat3 inverse_inertial_tensor = glm::zero<glm::mat3>();

        /**
         * inertia tensor local to the rigid body
         * constant value after initialization
         */
        glm::vec3 local_inertial_tensor = glm::zero<glm::vec3>();

        /**
         * inverse inertia tensor local to the rigid body
         * constant value after initialisation, derived from local inertial tensor
         */
        glm::vec3 local_inverse_inertial_tensor = glm::zero<glm::vec3>();
      };
    }
  }
}
