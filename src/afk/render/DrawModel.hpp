#pragma once

namespace afk {
  namespace render {
    namespace debug {
      enum class PhysicsView {
        /// Display the AABB for each collider
        COLLIDER_AABB = 1 << 0,
        /// Display the fat AABB of the broad phase collision detection for each collider
        COLLIDER_BROADPHASE_AABB = 1 << 1,
        /// Display the collision shape of each collider
        COLLISION_SHAPE = 1 << 2,
        /// Display the contact points
        CONTACT_POINT = 1 << 3,
        /// Display the contact normals
        CONTACT_NORMAL = 1 << 4,
      };
    }
  }
}
