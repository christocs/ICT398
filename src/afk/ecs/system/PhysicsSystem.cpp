#include "afk/ecs/system/PhysicsSystem.hpp"

#include "afk/Engine.hpp"
#include "afk/debug/Assert.hpp"
#include "afk/ecs/component/ColliderComponent.hpp"
#include "afk/ecs/component/PhysicsComponent.hpp"
#include "afk/ecs/component/TransformComponent.hpp"
#include "afk/io/Log.hpp"

using afk::ecs::component::ColliderComponent;
using afk::ecs::component::PhysicsComponent;
using afk::ecs::component::TransformComponent;
using afk::ecs::system::PhysicsSystem;
using afk::event::Event;
using afk::physics::Transform;
using afk::physics::shape::Box;
using afk::physics::shape::Capsule;
using afk::physics::shape::Sphere;

auto PhysicsSystem::update() -> void {
  auto &afk           = afk::Engine::get();
  auto &registry      = afk.ecs.registry;
  auto &event_manager = afk.event_manager;
  auto dt             = afk.get_delta_time();
  // only bother updating dynamic rigid bodies
  // any entity with both a collider component and a physics component is a dynamic rigid body
  const auto view =
      registry.view<ColliderComponent, PhysicsComponent, TransformComponent>();

  // process updates to each dynamic rigid body using semi-implicit euler integration
  for (const auto entity : view) {
    auto &physics   = registry.get<PhysicsComponent>(entity);
    auto &transform = registry.get<TransformComponent>(entity);
    // integrate velocity
    physics.linear_velocity += dt * physics.inverse_mass * physics.external_forces;
    // add gravity to linear velocity
    if (afk.gravity_enabled && false) {
      physics.linear_velocity += dt * afk.gravity;
    }

    // apply dampening
    const auto linear_dampening = std::pow(1.0f - physics.linear_dampening, dt);
    physics.linear_dampening *= linear_dampening;

    // apply velocity to translation AFTER it has been calculated for semi-implicit euler integration
    transform.translation += physics.linear_velocity * dt;

    // reset external forces and torque for the next update cycle
    // these only represent "moments" in acceleration
    physics.external_forces  = glm::vec3{0.0f};
    physics.external_torques = glm::vec3{0.0f};
  }
}

auto PhysicsSystem::collision_resolution_callback(Event event) -> void {
  // this method should only be processing Collision events and will assume the event is a collision event
  afk_assert(event.type == afk::event::Event::Type::Collision,
             "event type was not 'Collision'");

  auto &afk     = afk::Engine::get();
  const auto dt = afk.get_delta_time();
}
