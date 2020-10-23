#include "afk/ecs/system/PhysicsSystem.hpp"

#include "afk/Engine.hpp"
#include "afk/debug/Assert.hpp"
#include "afk/ecs/component/ColliderComponent.hpp"
#include "afk/ecs/component/PhysicsComponent.hpp"
#include "afk/ecs/component/TransformComponent.hpp"
#include "afk/io/Log.hpp"
#include "afk/io/Time.hpp"
#include "afk/utility/Visitor.hpp"

using afk::ecs::component::ColliderComponent;
using afk::ecs::component::PhysicsComponent;
using afk::ecs::component::TransformComponent;
using afk::ecs::system::PhysicsSystem;
using afk::event::Event;
using afk::physics::Transform;
using afk::physics::shape::Box;
using afk::physics::shape::Capsule;
using afk::physics::shape::Sphere;
using afk::utility::Visitor;

auto PhysicsSystem::initialize() -> void {
  afk_assert(!this->is_initialized, "Physics system already initialized");
  auto &engine = afk::Engine::get();

  engine.event_manager.register_event(
      Event::Type::Collision,
      event::EventManager::Callback{ecs::system::PhysicsSystem::collision_resolution_callback});

  this->is_initialized = true;
  afk::io::log << afk::io::get_date_time() << "Physics subsystem initialized\n";
}

auto PhysicsSystem::update() -> void {
  auto &afk           = afk::Engine::get();
  auto &registry      = afk.ecs.registry;
  auto &event_manager = afk.event_manager;
  auto dt             = afk.get_delta_time();
  // only bother updating rigid bodies
  const auto view =
      registry.view<ColliderComponent, PhysicsComponent, TransformComponent>();

  // process updates to each dynamic rigid body using semi-implicit euler integration
  for (const auto entity : view) {
    auto &physics   = registry.get<PhysicsComponent>(entity);
    auto &transform = registry.get<TransformComponent>(entity);

    // skip anything that is static
    if (!physics.is_static) {

      // add gravity as an external linear force
      if (afk.gravity_enabled) {
        physics.external_forces += afk.gravity;
      }

      // integrate linear velocity
      physics.linear_velocity += dt * physics.inverse_mass * physics.external_forces;

      // integrate angular velocity
      physics.angular_velocity += dt * physics.external_torques;

      // apply linear dampening
      const auto linear_dampening = std::pow(1.0f - physics.linear_dampening, dt);
      physics.linear_velocity *= linear_dampening;

      // apply angular dampening
      const auto angular_dampening = std::pow(1.0f - physics.angular_dampening, dt);
      physics.angular_velocity *= angular_dampening;

      // apply velocity to translation AFTER it has been calculated for semi-implicit euler integration
      transform.translation += physics.linear_velocity * dt;

      // reset external forces and torque for the next update cycle
      // these only represent "moments" in acceleration
      physics.external_forces  = glm::vec3{0.0f};
      physics.external_torques = glm::vec3{0.0f};
    }
  }
}

auto PhysicsSystem::instantiate_physics_component(const afk::ecs::Entity &entity,
                                                  PhysicsComponent &physics_component,
                                                  const ColliderComponent &collider_component)
    -> void {

  //// add all colliders to rp3d collision body and start calculating the average center of mass
  physics_component.center_of_mass = glm::vec3{0.0f};
  for (const auto &collision_body : collider_component.colliders) {
    //  // accumulate center of mass
    physics_component.center_of_mass += collision_body.center_of_mass;

    // calculate average center of mass after all the individual colliders'
    // centers of mass have been accumulated no point in dividing by 0 or 1
    if (collider_component.colliders.size() > 1) {
      physics_component.center_of_mass /= collider_component.colliders.size();
    }
  }
}

auto PhysicsSystem::collision_resolution_callback(Event event) -> void {
  // this method should only be processing Collision events and will assume the event is a collision event
  afk_assert(event.type == Event::Type::Collision,
             "event type was not 'Collision'");

  auto &afk      = afk::Engine::get();
  const auto dt  = afk.get_delta_time();
  auto &registry = afk.ecs.registry;

  afk::io::log << "collision physics callback called\n";

  auto visitor = Visitor{
      [dt, &afk, &registry](Event::Collision &c) {
        // only do physics resolution on entities that are not static
        if (registry.has<PhysicsComponent>(c.entity1) &&
            registry.has<PhysicsComponent>(c.entity2)) {

          auto &physics1 = registry.get<PhysicsComponent>(c.entity1);
          auto &physics2 = registry.get<PhysicsComponent>(c.entity2);

          // only do resolution if at least one of the entities has a non-static physics component
          if (!physics1.is_static || !physics2.is_static) {

            afk::io::log << "collision points:\n";
            for (auto i = size_t{0}; i < c.contacts.size(); ++i) {
              const auto &transform1 = registry.get<TransformComponent>(c.entity1);
              const auto &transform2 = registry.get<TransformComponent>(c.entity2);

              // @todo add more than just the translate
              const auto world_space1 =
                  transform1.translation + c.contacts[i].collider1_local_point;
              const auto world_space2 =
                  transform2.translation + c.contacts[i].collider2_local_point;
              afk::io::log << "\t1: local - x:" + std::to_string(world_space1.x) +
                                  ", y: " + std::to_string(world_space1.x) +
                                  ", z:" + std::to_string(world_space1.x) +
                                  "\n" + "\t2: local - x:" +
                                  std::to_string(world_space2.x) +
                                  ", y: " + std::to_string(world_space2.x) +
                                  ", z:" + std::to_string(world_space2.x) + "\n";
            }

            const auto &collider1 = registry.get<ColliderComponent>(c.entity1);
            const auto &collider2 = registry.get<ColliderComponent>(c.entity2);

            auto avg_normal = glm::vec3{0.0f};
            for (auto i = size_t{0}; i < c.contacts.size(); ++i) {
              avg_normal += c.contacts[i].normal;
            }

            // normal from item one to item two
            avg_normal /= c.contacts.size();

            const auto impulse_coefficient =
                PhysicsSystem::get_impulse_coefficient(c, avg_normal);

            const auto impulse = impulse_coefficient * avg_normal;

            // update forces and torque for collider 1 if it is not static
            // @todo update angular torque
            if (!physics1.is_static) {
              afk::io::log << "collision applied external forces 1\n";
              physics1.external_forces +=
                  impulse; // applying inverse mass is handled in a different function
            }
            // update forces and torque for collider 2 if it is not static
            // @todo update angular torque
            if (!physics2.is_static) {
              afk::io::log << "collision applied external forces 2\n";
              physics2.external_forces +=
                  impulse; // applying inverse mass is handled in a different function
            }
          }
        }
      },
      [](auto) { afk_assert(false, "Event data must be of type Collision"); }};

  std::visit(visitor, event.data);
}

// todo check if using contact normal should be used instead of collision normal
// todo check if a contact normal is being passed or a collision normal is being passed
auto PhysicsSystem::get_impulse_coefficient(const Event::Collision &data,
                                            const glm::vec3 &contact_normal) -> f32 {
  // @todo move this to a better place
  // 1 for fully elastic, 0 for no elastiscity at all
  const auto restitution_coefficient = 1.0f;

  auto &registry = afk::Engine::get().ecs.registry;
  auto collider1 = registry.get<ColliderComponent>(data.entity1);
  auto collider2 = registry.get<ColliderComponent>(data.entity2);

  const auto collider_1_physics = registry.get<PhysicsComponent>(data.entity1);
  const auto collider_2_physics = registry.get<PhysicsComponent>(data.entity2);

  // average points of collision relative to each rigid body's own local space
  auto avg_local_collision_point1 = glm::vec3{0.0f};
  auto avg_local_collision_point2 = glm::vec3{0.0f};
  for (auto i = size_t{0}; i < data.contacts.size(); ++i) {
    avg_local_collision_point1 += data.contacts[i].collider1_local_point;
    avg_local_collision_point2 += data.contacts[i].collider2_local_point;
  }
  avg_local_collision_point1 /= data.contacts.size();
  avg_local_collision_point2 /= data.contacts.size();

  // vectors from center of mass to collision points in local space
  // @todo check if this is meant to be using world space or local space, at the moment it is using local space
  const auto r1 = avg_local_collision_point1 - collider_1_physics.center_of_mass;
  const auto r2 = avg_local_collision_point1 - collider_2_physics.center_of_mass;

  // velocity before collision
  const auto v1 = collider_1_physics.linear_velocity;
  const auto v2 = collider_2_physics.linear_velocity;

  // rotational speed before collision
  const auto omega1 = collider_1_physics.angular_velocity;
  const auto omega2 = collider_2_physics.angular_velocity;

  // 1/(inertial tensor)
  const auto &j1 = collider_1_physics.inverse_inertial_tensor;
  const auto &j2 = collider_2_physics.inverse_inertial_tensor;

  // inverse mass
  // make the number as low as possible for static entities to make it appear like they're very heavy
  const auto &inverse_mass1 = collider_1_physics.inverse_mass;
  const auto &inverse_mass2 = collider_1_physics.inverse_mass;

  f32 numerator = glm::dot(contact_normal, v1 - v2) +
                  glm::dot(omega1, glm::cross(r1, contact_normal)) -
                  glm::dot(r2, contact_normal);
  numerator *= -(1 + restitution_coefficient);

  // @todo calculate using inverse of inertial tensor once the data is actually populated
  // f32 denominator =
  //    glm::dot(glm::cross(r1, contact_normal), j1 * glm::cross(r1, contact_normal));
  // denominator +=
  //    glm::dot(glm::cross(r2, contact_normal), j1 * glm::cross(r2, contact_normal));
  // denominator += inverse_mass1 + inverse_mass2;

  f32 denominator =
      glm::dot(glm::cross(r1, contact_normal), glm::cross(r1, contact_normal));
  denominator += glm::dot(glm::cross(r2, contact_normal), glm::cross(r2, contact_normal));
  denominator += inverse_mass1 + inverse_mass2;

  return numerator / denominator;
}
