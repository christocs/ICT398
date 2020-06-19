#include "afk/physics/PhysicsBody.hpp"

#include "afk/debug/Assert.hpp"

using Afk::PhysicsBody;

PhysicsBody::PhysicsBody(GameObject e, Afk::PhysicsBodySystem *physics_system,
                         Afk::Transform transform, float bounciness, float linear_dampening,
                         float angular_dampening, float mass, bool gravity_enabled,
                         Afk::RigidBodyType body_type, Afk::Box bounding_box) {
  this->owning_entity   = e;
  this->collision_shape = physics_system->physics_common.createBoxShape(rp3d::Vector3(
      bounding_box.x * transform.scale.x, bounding_box.y * transform.scale.y,
      bounding_box.z * transform.scale.z));

  this->body = physics_system->world->createRigidBody(rp3d::Transform(
      rp3d::Vector3(transform.translation[0], transform.translation[1],
                    transform.translation[2]),
      rp3d::Quaternion(transform.rotation[0], transform.rotation[1],
                       transform.rotation[2], transform.rotation[3])));

  this->body->enableGravity(gravity_enabled);

  switch (body_type) {
    case Afk::RigidBodyType::STATIC:
      this->body->setType(rp3d::BodyType::STATIC);
      break;
    case Afk::RigidBodyType::KINEMATIC:
      this->body->setType(rp3d::BodyType::KINEMATIC);
      break;
    case Afk::RigidBodyType::DYNAMIC:
      this->body->setType(rp3d::BodyType::DYNAMIC);
      break;
  }
  this->rigid_body_type = body_type;

  afk_assert(linear_dampening >= 0, "Linear dampening cannot be negative");
  this->body->setLinearDamping(static_cast<rp3d::decimal>(linear_dampening));
  afk_assert(angular_dampening >= 0, "Angular dampening cannot be negative");
  this->body->setAngularDamping(static_cast<rp3d::decimal>(angular_dampening));

  this->body->setMass(mass);

  this->collider = this->body->addCollider(this->collision_shape,
                                                 rp3d::Transform::identity());

  afk_assert(bounciness >= 0 && bounciness <= 1,
             "Bounciness must be between 0 and 1");
  this->collider->getMaterial().setBounciness(static_cast<rp3d::decimal>(bounciness));
}

PhysicsBody::PhysicsBody(GameObject e, Afk::PhysicsBodySystem *physics_system,
                         Afk::Transform transform, float bounciness, float linear_dampening,
                         float angular_dampening, float mass, bool gravity_enabled,
                         Afk::RigidBodyType body_type, Afk::Sphere bounding_sphere) {
  this->owning_entity = e;
  // Note: have to scale sphere equally on every axis (otherwise it wouldn't be a sphere), so scaling the average of each axis
  const auto scaleFactor =
      (transform.scale.x + transform.scale.y + transform.scale.z) / 3.0f;
  this->collision_shape = physics_system->physics_common.createSphereShape(bounding_sphere * scaleFactor);

  this->body = physics_system->world->createRigidBody(rp3d::Transform(
      rp3d::Vector3(transform.translation[0], transform.translation[1],
                    transform.translation[2]),
      rp3d::Quaternion(transform.rotation[0], transform.rotation[1],
                       transform.rotation[2], transform.rotation[3])));

  this->body->enableGravity(gravity_enabled);

  switch (body_type) {
    case Afk::RigidBodyType::STATIC:
      this->body->setType(rp3d::BodyType::STATIC);
      break;
    case Afk::RigidBodyType::KINEMATIC:
      this->body->setType(rp3d::BodyType::KINEMATIC);
      break;
    case Afk::RigidBodyType::DYNAMIC:
      this->body->setType(rp3d::BodyType::DYNAMIC);
      break;
  }
  this->rigid_body_type = body_type;

  afk_assert(linear_dampening >= 0, "Linear dampening cannot be negative");
  this->body->setLinearDamping(static_cast<rp3d::decimal>(linear_dampening));
  afk_assert(angular_dampening >= 0, "Angular dampening cannot be negative");
  this->body->setAngularDamping(static_cast<rp3d::decimal>(angular_dampening));

  this->body->setMass(mass);

  this->collider =
      this->body->addCollider(this->collision_shape, rp3d::Transform::identity());

  afk_assert(bounciness >= 0 && bounciness <= 1,
             "Bounciness must be between 0 and 1");
  this->collider->getMaterial().setBounciness(static_cast<rp3d::decimal>(bounciness));
}

PhysicsBody::PhysicsBody(GameObject e, Afk::PhysicsBodySystem *physics_system,
                         Afk::Transform transform, float bounciness, float linear_dampening,
                         float angular_dampening, float mass, bool gravity_enabled,
                         Afk::RigidBodyType body_type, Afk::Capsule bounding_capsule) {
  this->owning_entity = e;
  // Note: have to scale sphere equally on x-z axis
  this->collision_shape = physics_system->physics_common.createCapsuleShape(
      bounding_capsule.radius * ((transform.scale.x + transform.scale.y) / 2.0f),
      bounding_capsule.height * transform.scale.y);

  this->body = physics_system->world->createRigidBody(rp3d::Transform(
      rp3d::Vector3(transform.translation[0], transform.translation[1],
                    transform.translation[2]),
      rp3d::Quaternion(transform.rotation[0], transform.rotation[1],
                       transform.rotation[2], transform.rotation[3])));

  this->body->enableGravity(gravity_enabled);

  switch (body_type) {
    case Afk::RigidBodyType::STATIC:
      this->body->setType(rp3d::BodyType::STATIC);
      break;
    case Afk::RigidBodyType::KINEMATIC:
      this->body->setType(rp3d::BodyType::KINEMATIC);
      break;
    case Afk::RigidBodyType::DYNAMIC:
      this->body->setType(rp3d::BodyType::DYNAMIC);
      break;
  }
  this->rigid_body_type = body_type;

  afk_assert(linear_dampening >= 0, "Linear dampening cannot be negative");
  this->body->setLinearDamping(static_cast<rp3d::decimal>(linear_dampening));
  afk_assert(angular_dampening >= 0, "Angular dampening cannot be negative");
  this->body->setAngularDamping(static_cast<rp3d::decimal>(angular_dampening));

  this->body->setMass(mass);

  this->collider =
      this->body->addCollider(this->collision_shape, rp3d::Transform::identity());

  afk_assert(bounciness >= 0 && bounciness <= 1,
             "Bounciness must be between 0 and 1");
  this->collider->getMaterial().setBounciness(static_cast<rp3d::decimal>(bounciness));
}

PhysicsBody::PhysicsBody(GameObject e, Afk::PhysicsBodySystem *physics_system,
                         Afk::Transform transform, float bounciness,
                         float linear_dampening, float angular_dampening, float mass,
                         bool gravity_enabled, Afk::RigidBodyType body_type,
                         const Afk::HeightMap &height_map) {
  this->owning_entity = e;

  auto max_height = 0.0f;
  auto min_height = 0.0f;
  for (auto height : height_map.heights) {
    if (height > max_height) {
      max_height = height;
    } else if (height < min_height) {
      min_height = height;
    }
  }
  this->collision_shape = physics_system->physics_common.createHeightFieldShape(
      height_map.width,
      (static_cast<int>(height_map.heights.size()) / height_map.width),
      min_height, max_height, height_map.heights.data(),
      rp3d::HeightFieldShape::HeightDataType::HEIGHT_FLOAT_TYPE);

  auto temp1 = rp3d::Vector3{-9999, -9999, -9999};
  auto temp2 = rp3d::Vector3{0, 0, 0};
  this->collision_shape->getLocalBounds(temp1, temp2);

  this->body = physics_system->world->createRigidBody(rp3d::Transform(
      rp3d::Vector3(transform.translation[0], transform.translation[1],
                    transform.translation[2]),
      rp3d::Quaternion(transform.rotation[0], transform.rotation[1],
                       transform.rotation[2], transform.rotation[3])));

  this->body->enableGravity(gravity_enabled);

  switch (body_type) {
    case Afk::RigidBodyType::STATIC:
      this->body->setType(rp3d::BodyType::STATIC);
      break;
    case Afk::RigidBodyType::KINEMATIC:
      this->body->setType(rp3d::BodyType::KINEMATIC);
      break;
    case Afk::RigidBodyType::DYNAMIC:
      this->body->setType(rp3d::BodyType::DYNAMIC);
      break;
  }
  this->rigid_body_type = body_type;

  afk_assert(linear_dampening >= 0, "Linear dampening cannot be negative");
  this->body->setLinearDamping(static_cast<rp3d::decimal>(linear_dampening));
  afk_assert(angular_dampening >= 0, "Angular dampening cannot be negative");
  this->body->setAngularDamping(static_cast<rp3d::decimal>(angular_dampening));

  this->body->setMass(mass);

  this->collider =
      this->body->addCollider(this->collision_shape, rp3d::Transform::identity());

  afk_assert(bounciness >= 0 && bounciness <= 1,
             "Bounciness must be between 0 and 1");
  this->collider->getMaterial().setBounciness(static_cast<rp3d::decimal>(bounciness));
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

void PhysicsBody::apply_force(glm::vec3 force) {
  this->body->applyForceToCenterOfMass(rp3d::Vector3{force.x, force.y, force.z});
}

void PhysicsBody::apply_torque(glm::vec3 torque) {
  this->body->applyTorque(rp3d::Vector3{torque.x, torque.y, torque.z});
}

Afk::RigidBodyType PhysicsBody::get_type() const {
  return rigid_body_type;
}
