#include "afk/physics/PhysicsBody.hpp"

using afk::physics::PhysicsBody;

PhysicsBody::PhysicsBody(GameObject e, PhysicsBodySystem *physics_system,
                         Transform transform, Box bounding_box) {
  this->owning_entity = e;
  this->collision_shape = physics_system->physics_common.createBoxShape(rp3d::Vector3(
      bounding_box.x * transform.scale.x, bounding_box.y * transform.scale.y,
      bounding_box.z * transform.scale.z));

  this->body = physics_system->world->createRigidBody(rp3d::Transform(
      rp3d::Vector3(transform.translation.x, transform.translation.y,
                    transform.translation.z),
      rp3d::Quaternion(transform.rotation.x, transform.rotation.y,
                       transform.rotation.z, transform.rotation.w)));

  this->body->setType(rp3d::BodyType::STATIC);

  this->collider =
      this->body->addCollider(this->collision_shape, rp3d::Transform::identity());
  this->collider->setIsTrigger(true);
}

PhysicsBody::PhysicsBody(GameObject e, PhysicsBodySystem *physics_system,
                         Transform transform, Sphere bounding_sphere) {
  this->owning_entity = e;
  // Note: have to scale sphere equally on every axis (otherwise it wouldn't be a sphere), so scaling the average of each axis
  const auto scaleFactor =
      (transform.scale.x + transform.scale.y + transform.scale.z) / 3.0f;

  this->body = physics_system->world->createRigidBody(rp3d::Transform(
      rp3d::Vector3(transform.translation.x, transform.translation.y,
                    transform.translation.z),
      rp3d::Quaternion(transform.rotation[0], transform.rotation[1],
                       transform.rotation[2], transform.rotation.w)));

  this->collision_shape =
      physics_system->physics_common.createSphereShape(bounding_sphere * scaleFactor);

  this->body->setType(rp3d::BodyType::STATIC);

  this->collider =
      this->body->addCollider(this->collision_shape, rp3d::Transform::identity());
  this->collider->setIsTrigger(true);
}

PhysicsBody::PhysicsBody(GameObject e, PhysicsBodySystem *physics_system,
                         Transform transform, Capsule bounding_capsule) {
  this->owning_entity = e;
  // Note: have to scale sphere equally on x-z axis
  this->collision_shape = physics_system->physics_common.createCapsuleShape(
      bounding_capsule.radius * ((transform.scale.x + transform.scale.y) / 2.0f),
      bounding_capsule.height * transform.scale.y);

  this->body = physics_system->world->createRigidBody(rp3d::Transform(
      rp3d::Vector3(transform.translation.x, transform.translation.y,
                    transform.translation.z),
      rp3d::Quaternion(transform.rotation[0], transform.rotation[1],
                       transform.rotation[2], transform.rotation.w)));

  this->body->setType(rp3d::BodyType::STATIC);

  this->collider =
      this->body->addCollider(this->collision_shape, rp3d::Transform::identity());
  this->collider->setIsTrigger(true);
}

PhysicsBody::PhysicsBody(GameObject e, PhysicsBodySystem *physics_system,
                         Transform transform, const HeightMap &height_map) {
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

//  auto temp1 = rp3d::Vector3{-9999, -9999, -9999};
//  auto temp2 = rp3d::Vector3{0, 0, 0};
//  this->collision_shape->getLocalBounds(temp1, temp2);

  this->body = physics_system->world->createRigidBody(rp3d::Transform(
      rp3d::Vector3(transform.translation.x, transform.translation.y,
                    transform.translation.z),
      rp3d::Quaternion(transform.rotation[0], transform.rotation[1],
                       transform.rotation[2], transform.rotation.w)));

  this->body->setType(rp3d::BodyType::STATIC);

  this->collider =
      this->body->addCollider(this->collision_shape, rp3d::Transform::identity());
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
