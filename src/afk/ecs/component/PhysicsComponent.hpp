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
        struct LinearState {
          struct Derivative {
            glm::vec3 velocity = glm::vec3{0.0f};
            glm::vec3 force = glm::vec3{0.0f};
          };

          // primary values
          glm::vec3 position = glm::vec3{0.0f};
          glm::vec3 momentum = glm::vec3{0.0f};

          // secondary values (derived from primary and constant values)
          glm::vec3 velocity = glm::vec3{0.0f};

          // constant values
          f32 mass;
          f32 inverse_mass;

          // todo move this out of component, components should strictly contain data and no methods
          // should be called whenever any of the primary values change
          void recalculate() {
            velocity = momentum * inverse_mass;
          }
        };

        struct AngularState {
          struct Derivative {
            glm::quat spin;
            glm::vec3 torque;
          };

          // primary values
          glm::quat orientation;
          glm::vec3 angular_momentum;

          // secondary values (derived from primary and constant values)
          glm::quat spin;
          glm::vec3 angular_velocity;

          // constant values
          f32 inertia;
          f32 inverse_inertia;

          // todo move this out of component, components should strictly contain data and no methods
          // should be called whenever any of the primary values change
          void recalculate() {
            angular_velocity = angular_momentum * inverse_inertia;

            // normalisation isn't required for every update cycle, if cpu cycles are tight, this just keeps orientation from drifting
            // doing it on every update cycle should be more accurate
            orientation = glm::normalize(orientation);

            const auto q = glm::quat{0, angular_velocity.x, angular_velocity.y, angular_velocity.z};

            spin = 0.5f * q * orientation;
          }
        };

        LinearState linear_state;
        AngularState angular_state;
        glm::vec3 center_of_mass;
      };
    }
  }
}
