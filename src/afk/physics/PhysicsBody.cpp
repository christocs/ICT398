#include "afk/physics/PhysicsBody.hpp"

#include "afk/Afk.hpp"
#include "afk/debug/Assert.hpp"

using afk::physics::PhysicsBody;

PhysicsBody::PhysicsBody(GameObject e, PhysicsBodySystem *physics_system,
                         const Transform &transform,
                         const CollisionBodyCollection &collision_bodies) {
  this->owning_entity = e;

  this->body = physics_system->world->createRigidBody(rp3d::Transform(
      rp3d::Vector3(transform.translation.x, transform.translation.y,
                    transform.translation.z),
      rp3d::Quaternion(transform.rotation.x, transform.rotation.y,
                       transform.rotation.z, transform.rotation.w)));

  this->body->setType(rp3d::BodyType::STATIC);

  for (const auto &collision_body : collision_bodies) {
    // do NOT allocate memory yourself, let reactphysics3d handle it
    rp3d::CollisionShape *rp3d_collision_shape = nullptr;

    // avoid proper runtime checking of types
    if (collision_body.type == CollisionBodyType::Box) {
      rp3d_collision_shape = this->createShapeBox(
          std::get<shape::Box>(collision_body.body), collision_body.transform);
    } else if (collision_body.type == CollisionBodyType::Sphere) {
      rp3d_collision_shape = this->createShapeSphere(
          std::get<shape::Sphere>(collision_body.body), collision_body.transform);
    } else if (collision_body.type == CollisionBodyType::Capsule) {
      rp3d_collision_shape = this->createShapeCapsule(
          std::get<shape::Capsule>(collision_body.body), collision_body.transform);
    } else {
        afk_assert(false, "Exhausted collision body types");
    }

    this->collider =
        this->body->addCollider(rp3d_collision_shape, rp3d::Transform::identity());
  }

  this->collider->setIsTrigger(true);
}

void PhysicsBody::translate(glm::vec3 translate) {
  this->body->setTransform(rp3d::Transform{
      rp3d::Vector3{this->body->getTransform().getPosition().x + translate.x,
                    this->body->getTransform().getPosition().y + translate.y,
                    this->body->getTransform().getPosition().z + translate.z},
      this->body->getTransform().getOrientation()});
}

void PhysicsBody::set_pos(glm::vec3 pos) {
  this->body->setTransform(rp3d::Transform{rp3d::Vector3{pos.x, pos.y, pos.z},
                                           this->body->getTransform().getOrientation()});
}

rp3d::BoxShape *PhysicsBody::createShapeBox(const afk::physics::shape::Box &box,
                                 const afk::physics::Transform &transform) {
  auto &engine = afk::Engine::get();
  return engine.physics_body_system.physics_common.createBoxShape(rp3d::Vector3(
    box.x * transform.scale.x, box.y * transform.scale.y,
    box.z * transform.scale.z));
}

rp3d::SphereShape *PhysicsBody::createShapeSphere(const afk::physics::shape::Sphere &sphere,
                                 const afk::physics::Transform &transform) {
  // Note: have to scale sphere equally on every axis (otherwise it wouldn't be a sphere), so scaling the average of each axis
  const auto scale_factor =
      (transform.scale.x + transform.scale.y + transform.scale.z) / 3.0f;

  auto &engine = afk::Engine::get();
  return engine.physics_body_system.physics_common.createSphereShape(sphere * scale_factor);
}

rp3d::CapsuleShape *PhysicsBody::createShapeCapsule(const afk::physics::shape::Capsule &capsule,
                                 const afk::physics::Transform &transform) {
  auto &engine = afk::Engine::get();
  return engine.physics_body_system.physics_common.createCapsuleShape(
      capsule.radius * ((transform.scale.x + transform.scale.y) / 2.0f),
      capsule.height * transform.scale.y);
}