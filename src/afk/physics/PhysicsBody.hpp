#pragma once

#include <memory>

#include <entt/entt.hpp>
#include <reactphysics3d/reactphysics3d.h>

#include "afk/component/BaseComponent.hpp"
#include "afk/physics/PhysicsBodySystem.hpp"
#include "afk/physics/RigidBodyType.hpp"
#include "afk/physics/Transform.hpp"
#include "afk/physics/shape/Box.hpp"
#include "afk/physics/shape/Capsule.hpp"
#include "afk/physics/shape/HeightMap.hpp"
#include "afk/physics/shape/Sphere.hpp"
#include "glm/vec3.hpp"

namespace Afk {
  class PhysicsBodySystem;

  using RigidBody      = rp3d::RigidBody;
  using Collider       = rp3d::Collider;
  using CollisionShape = rp3d::CollisionShape;

  class PhysicsBody : public BaseComponent {
  public:
    PhysicsBody() = delete;

    /**
     * constructor
     * \todo Builder method maybe?
     */
    PhysicsBody(GameObject e, Afk::PhysicsBodySystem *physics_system,
                Afk::Transform transform, float bounciness, float linear_dampening,
                float angular_dampening, float mass, bool gravity_enabled,
                Afk::RigidBodyType body_type, Afk::Box bounding_box);

    /**
     * constructor
     * \todo Builder method maybe?
     */
    PhysicsBody(GameObject e, Afk::PhysicsBodySystem *physics_system,
                Afk::Transform transform, float bounciness, float linear_dampening,
                float angular_dampening, float mass, bool gravity_enabled,
                Afk::RigidBodyType body_type, Afk::Sphere bounding_sphere);

    /**
     * constructor
     * \todo Builder method maybe?
     */
    PhysicsBody(GameObject e, Afk::PhysicsBodySystem *physics_system,
                Afk::Transform transform, float bounciness, float linear_dampening,
                float angular_dampening, float mass, bool gravity_enabled,
                Afk::RigidBodyType body_type, Afk::Capsule bounding_capsule);
    /**
     * constructor
     * \todo Builder method maybe?
     */
    PhysicsBody(GameObject e, Afk::PhysicsBodySystem *physics_system,
                Afk::Transform transform, float bounciness, float linear_dampening,
                float angular_dampening, float mass, bool gravity_enabled,
                Afk::RigidBodyType body_type, const Afk::HeightMap &height_map);

    // todo add rotate method

    /// translate the position of the physics body
    void translate(glm::vec3 translate);

    /// set position of body 
    void set_pos(glm::vec3 pos);

    /// set velocity on the body
    void set_velocity(glm::vec3 velocity);

    /// apply force to the centre of mass of the body for the next physics update
    void apply_force(glm::vec3 force);

    /// apply torque for the next physics update
    void apply_torque(glm::vec3 force);

    /// get rigid body type
    Afk::RigidBodyType get_type() const;

  private:
    RigidBody *body                 = nullptr;
    Collider *collider              = nullptr;
    CollisionShape *collision_shape = nullptr;

    Afk::RigidBodyType rigid_body_type;

    friend class PhysicsBodySystem;
  };
}
