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

        /**
         * if the component is static
         * is constant after initialisation
         */
        bool is_static = true;

        /**
         * center of mass local to the entity
         * is constant after initialisation
         */
        glm::vec3 center_of_mass = glm::vec3{};

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
         * mass in kilograms, derrived from adding all masses of colliders
         * together is constant after initialisation
         */
        f32 total_mass = {};

        /**
         * inverse of mass, derrived from 1 / total_mass
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
         * inertial tensor in world rotation
         * changesg on each update depending on the local inertia tensor and the rotation of the entity
         */
        glm::mat3 inverse_inertial_tensor = glm::zero<glm::mat3>();

        /**
         * inertial tensor local to the rigid body
         * constant value after initialization
         */
        glm::mat3 local_inertial_tensor = glm::zero<glm::mat3>();

        /**
         * inverse inertial tensor local to the rigid body
         * constant value after initialisation, derrived from local inertial tensor
         */
        glm::mat3 local_inverse_inertial_tensor = glm::mat3x3{1.0f};
      };
    }
  }
}
