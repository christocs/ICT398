#pragma once

#include <memory>
#include <variant>
#include <vector>

#include <entt/entt.hpp>
#include <reactphysics3d/reactphysics3d.h>

#include "afk/component/BaseComponent.hpp"
#include "afk/physics/PhysicsBodySystem.hpp"
#include "afk/physics/Transform.hpp"
#include "afk/physics/shape/Box.hpp"
#include "afk/physics/shape/Capsule.hpp"
#include "afk/physics/shape/Sphere.hpp"
#include "glm/vec3.hpp"

namespace afk {
  namespace physics {
    class PhysicsBodySystem;

    using CollisionBodyVariant =
        std::variant<afk::physics::shape::Box, afk::physics::shape::Sphere, afk::physics::shape::Capsule>;

    enum CollisionBodyType { Box, Sphere, Capsule };

    struct CollisionBody {
      CollisionBodyType type            = {};
      CollisionBodyVariant body         = {};
      afk::physics::Transform transform = {}; // transform relative to parent
    };

    using CollisionBodyCollection = std::vector<CollisionBody>;

    class PhysicsBody : public afk::BaseComponent {
    public:
      PhysicsBody() = delete;

      /**
       * constructor
       */
      PhysicsBody(GameObject body, PhysicsBodySystem *physics_system,
                  const afk::physics::Transform &transform,
                  const CollisionBodyCollection &collision_bodies);

      // todo add rotate method

      /// translate the position of the physics body
      void translate(glm::vec3 translate);

      /// set position of body
      void set_pos(glm::vec3 pos);

    private:
      rp3d::BoxShape *createShapeBox(const afk::physics::shape::Box &shape,
                                     const afk::physics::Transform &transform);

      rp3d::SphereShape *createShapeSphere(const afk::physics::shape::Sphere &shape,
                                           const afk::physics::Transform &transform);

      rp3d::CapsuleShape *createShapeCapsule(const afk::physics::shape::Capsule &shape,
                                             const afk::physics::Transform &transform);

      rp3d::RigidBody *body                 = nullptr;
      rp3d::Collider *collider              = nullptr;
      rp3d::CollisionShape *collision_shape = nullptr;

      friend class PhysicsBodySystem;
    };
  }
}
