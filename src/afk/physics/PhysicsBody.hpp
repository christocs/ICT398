#pragma once

#include <memory>

#include <entt/entt.hpp>
#include <reactphysics3d/reactphysics3d.h>

#include "afk/component/BaseComponent.hpp"
#include "afk/physics/PhysicsBodySystem.hpp"
#include "afk/physics/Transform.hpp"
#include "afk/physics/shape/Box.hpp"
#include "afk/physics/shape/Capsule.hpp"
#include "afk/physics/shape/HeightMap.hpp"
#include "afk/physics/shape/Sphere.hpp"
#include "glm/vec3.hpp"

namespace afk {
  namespace physics {
    class PhysicsBodySystem;

    using RigidBody      = rp3d::RigidBody;
    using Collider       = rp3d::Collider;
    using CollisionShape = rp3d::CollisionShape;

    using afk::GameObject;
    using afk::physics::PhysicsBodySystem;
    using afk::physics::Transform;
    using afk::physics::shape::Box;
    using afk::physics::shape::Capsule;
    using afk::physics::shape::HeightMap;
    using afk::physics::shape::Sphere;

    class PhysicsBody : public afk::BaseComponent {
    public:
      PhysicsBody() = delete;

      /**
       * constructor
       * \todo Builder method maybe?
       */
      PhysicsBody(GameObject e, PhysicsBodySystem *physics_system,
                  Transform transform, Box bounding_box);

      /**
       * constructor
       * \todo Builder method maybe?
       */
      PhysicsBody(GameObject e, PhysicsBodySystem *physics_system,
                  Transform transform, Sphere bounding_sphere);

      /**
       * constructor
       * \todo Builder method maybe?
       */
      PhysicsBody(GameObject e, PhysicsBodySystem *physics_system,
                  Transform transform, Capsule bounding_capsule);
      /**
       * constructor
       * \todo Builder method maybe?
       */
      PhysicsBody(GameObject e, PhysicsBodySystem *physics_system,
                  Transform transform, const HeightMap &height_map);

      // todo add rotate method

      /// translate the position of the physics body
      void translate(glm::vec3 translate);

      /// set position of body
      void set_pos(glm::vec3 pos);

    private:
      RigidBody *body                 = nullptr;
      Collider *collider              = nullptr;
      CollisionShape *collision_shape = nullptr;

      friend class PhysicsBodySystem;
    };
  }
}
