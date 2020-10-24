#pragma once

#include <variant>
#include <vector>

#include <glm/glm.hpp>
#include "afk/NumericTypes.hpp"
#include "afk/physics/shape/Box.hpp"
#include "afk/physics/shape/Sphere.hpp"
#include "afk/physics/Transform.hpp"

namespace afk {
  namespace ecs {
    namespace component {
      /**
       * Encapsulates a collider component.
       */
      struct ColliderComponent {
        /** Collision shape variant definition, defining the possible physics shapes of a collider */
        using ColliderShape =
            std::variant<afk::physics::shape::Box, afk::physics::shape::Sphere>;

        /** A collider body is made up of a collision body as well as a transform local to the entity */
        struct Collider {
          /** Shape of the collider */
          ColliderShape shape                = {};
          /** Transform within local space of the entity */
          afk::physics::Transform transform = {};
          /** Mass of the collider */
          f32 mass = {};
        };

        /** Defining a collection of colliders */
        using ColliderCollection = std::vector<Collider>;

        /** Collecition of colliders for the entity */
        ColliderCollection colliders = {};
      };
    }
  }
}
