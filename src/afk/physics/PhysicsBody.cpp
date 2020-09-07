#include "afk/physics/PhysicsBody.hpp"

#include "afk/Afk.hpp"
#include "afk/debug/Assert.hpp"

using afk::physics::PhysicsBody;

PhysicsBody::PhysicsBody(entt::entity e, PhysicsBodySystem *physics_system,
                         const Transform &transform,
                         const CollisionBodyCollection &collision_bodies, BodyType _type) {
  // store which entt game object PhysicsBody belongs to
  this->owning_entity = e;

  // create collision body
  this->body = physics_system->world->createCollisionBody(rp3d::Transform(
      rp3d::Vector3(transform.translation.x, transform.translation.y,
                    transform.translation.z),
      rp3d::Quaternion(transform.rotation.x, transform.rotation.y,
                       transform.rotation.z, transform.rotation.w)));

  // create a map that lets you retrieve the entt object id from the collision body's object id
  auto rp3d_body_id = this->body->getEntity().id;
  auto rp3d_body_to_ecs_map = &afk::Engine::get().physics_body_system.rp3d_body_to_ecs_map;
  afk_assert(rp3d_body_to_ecs_map->count(rp3d_body_id) < 1, "AFK game object already has a physics body assigned");
  rp3d_body_to_ecs_map->insert({rp3d_body_id, e});

  // store if the PhysicsBody is static or dynamic (moving or not moving)
  this->type = _type;

  // add colliders (NOT collision bodies) to the collision body
  for (const auto &collision_body : collision_bodies) {
    // do NOT allocate memory yourself, let reactphysics3d handle it
    rp3d::CollisionShape *rp3d_collision_shape = nullptr;

    // need to apply parent scale at the shape level, as scale cannot be applied to the parent body level
    auto collision_transform = collision_body.transform;
    collision_transform.scale.x *= transform.scale.x;
    collision_transform.scale.y *= transform.scale.y;
    collision_transform.scale.z *= transform.scale.z;

    // todo: use std::visit for runtime type checking
    // create correct collider shape
    if (collision_body.type == CollisionBodyType::Box) {
      rp3d_collision_shape = afk::physics::PhysicsBody::createShapeBox(
          std::get<shape::Box>(collision_body.body), collision_transform.scale);
    } else if (collision_body.type == CollisionBodyType::Sphere) {
      rp3d_collision_shape = afk::physics::PhysicsBody::createShapeSphere(
          std::get<shape::Sphere>(collision_body.body), collision_transform.scale);
    } else if (collision_body.type == CollisionBodyType::Capsule) {
      rp3d_collision_shape = afk::physics::PhysicsBody::createShapeCapsule(
          std::get<shape::Capsule>(collision_body.body), collision_transform.scale);
    } else {
      afk_assert(false, "Exhausted collision body types");
    }

    // create transform for collider (this does NOT include scale as reactphysics does not have scale in its transform, to get around this the scale is manually added to the collision shapes)
    const auto rp3d_transform =
        rp3d::Transform(rp3d::Vector3(collision_transform.translation.x,
                                      collision_transform.translation.y,
                                      collision_transform.translation.z),
                        rp3d::Quaternion(collision_transform.rotation.x,
                                         collision_transform.rotation.y,
                                         collision_transform.rotation.z,
                                         collision_transform.rotation.w));

    // add the collider you just made to the collision body
    this->body->addCollider(rp3d_collision_shape, rp3d_transform);
  }
}

// translate the physics body's representation in the react physics world
void PhysicsBody::translate(glm::vec3 translate) {
  this->body->setTransform(rp3d::Transform{
      rp3d::Vector3{this->body->getTransform().getPosition().x + translate.x,
                    this->body->getTransform().getPosition().y + translate.y,
                    this->body->getTransform().getPosition().z + translate.z},
      this->body->getTransform().getOrientation()});
}

// set the position of the physics body's representation in the react physics world
void PhysicsBody::set_pos(glm::vec3 pos) {
  this->body->setTransform(rp3d::Transform{rp3d::Vector3{pos.x, pos.y, pos.z},
                                           this->body->getTransform().getOrientation()});
}

// create a box collision shape
rp3d::BoxShape *PhysicsBody::createShapeBox(const afk::physics::shape::Box &box,
                                            const glm::vec3 &scale) {
  auto &engine = afk::Engine::get();
  return engine.physics_body_system.physics_common.createBoxShape(
      rp3d::Vector3(box.x * scale.x, box.y * scale.y, box.z * scale.z));
}

// create a sphere collision shape
rp3d::SphereShape *PhysicsBody::createShapeSphere(const afk::physics::shape::Sphere &sphere,
                                                  const glm::vec3 &scale) {
  // Note: have to scale sphere equally on every axis (otherwise it wouldn't be a sphere), so scaling the average of each axis
  const auto scale_factor = (scale.x + scale.y + scale.z) / 3.0f;

  auto &engine = afk::Engine::get();
  return engine.physics_body_system.physics_common.createSphereShape(sphere * scale_factor);
}

// create a capsule collision shape
rp3d::CapsuleShape *PhysicsBody::createShapeCapsule(const afk::physics::shape::Capsule &capsule,
                                                    const glm::vec3 &scale) {
  auto &engine = afk::Engine::get();
  auto shape   = engine.physics_body_system.physics_common.createCapsuleShape(
      capsule.radius * ((scale.x + scale.y) / 2.0f), capsule.height * scale.y);
  return shape;
}
