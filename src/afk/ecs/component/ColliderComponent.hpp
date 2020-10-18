#pragma once

#include <variant>

#include "afk/physics/shape/Box.hpp"
#include "afk/physics/shape/Capsule.hpp"
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
            std::variant<afk::physics::shape::Box, afk::physics::shape::Sphere, afk::physics::shape::Capsule>;

        /**
         * Collider type
         *
         * A dynamic collider has physics applied to it and interacts with static and dynamic objects.
         * Astatic object will never move unless manually moved.
         */
        enum class ColliderType { Static, Dynamic };

        /** A collider body is made up of a collision body as well as a transform local to the entity */
        struct Collider {
          ColliderShape shape                = {};
          /** Transform relative to parent */
          afk::physics::Transform transform = {};
        };

        /** Defining a collection of colliders */
        using ColliderCollection = std::vector<Collider>;

        /** Collecition of colliders for the entity */
        ColliderCollection colliders = {};

        /** Transform of the component relative to global coordinates
         * @todo Transform component on entities and use that Transform instead, instead of using this duplicate
         * will require prefabs to be loaded in multi-pass or force the Transform to be loaded first
         */
        physics::Transform transform = {};

        // todo: consider importing mass, centre of mass, elasticity, etc., though that may be more appropriate in a separate component
      };
    }
  }
}
