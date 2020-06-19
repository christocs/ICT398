#include "afk/physics/PhysicsBodySystem.hpp"

using Afk::PhysicsBodySystem;

PhysicsBodySystem::PhysicsBodySystem(glm::vec3 gravity) {
  // TODO: remove temporary variable
  auto gravity_rp3d = rp3d::Vector3{gravity.x, gravity.y, gravity.z};
  this->world->setGravity(gravity_rp3d);
}

auto PhysicsBodySystem::get_gravity() {
  return glm::vec3{this->world->getGravity().x, this->world->getGravity().y,
                   this->world->getGravity().z};
}

auto PhysicsBodySystem::set_gravity(glm::vec3 gravity) {
  // TODO: remove temporary variable
  auto gravity_rp3d = rp3d::Vector3{gravity.x, gravity.y, gravity.z};
  this->world->setGravity(gravity_rp3d);
}

auto PhysicsBodySystem::update(entt::registry *registry, float dt) -> void {
  this->world->update(dt);

  // Mirror changes in physics engine to Transform component
  // TODO: Scale shapes of rigid bodies on the fly, updates in v0.8.0 might help with this
  // @see https://github.com/DanielChappuis/reactphysics3d/issues/103
  registry->view<Afk::Transform, Afk::PhysicsBody>().each(
      [](Afk::Transform &transform, Afk::PhysicsBody &collision) {
        const auto& rp3d_position = collision.body->getTransform().getPosition();
        const auto& rp3d_orientation = collision.body->getTransform().getOrientation();

        transform.translation =
            glm::vec3{rp3d_position.x, rp3d_position.y, rp3d_position.z};
        transform.rotation = glm::quat{rp3d_orientation.w, rp3d_orientation.x,
                                       rp3d_orientation.y, rp3d_orientation.z};
      });
}
