#include "afk/ecs/system/PhysicsSystem.hpp"

#include <limits>

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

      // integrate gravity acceleration
      if (afk.gravity_enabled) {
        physics.linear_velocity += dt * afk.gravity;
      }

      // integrate linear velocity
      physics.linear_velocity += physics.total_inverse_mass * physics.external_forces;
      physics.linear_velocity += dt * physics.total_inverse_mass * physics.external_forces;

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

  // set values to zero before iterating through each collider for calculations
  // assumes each collider's center of mass is always at the center of the
  // collider assume each collider has an even distribution of mass
  physics_component.center_of_mass = glm::vec3{0.0f};
  physics_component.total_mass     = 0.0f;
  auto temp_inertia_tensor         = glm::zero<glm::mat3>();
  for (const auto &collision_body : collider_component.colliders) {
    physics_component.total_mass += collision_body.mass;

    // accumulate center of mass, then divide it later when the total mass is known
    physics_component.center_of_mass =
        collision_body.transform.translation * collision_body.mass;

    auto collider_inertia_tensor = glm::vec3{};
    f32 collider_volume          = 0.0f;

    auto visitor = Visitor{
        [&collider_inertia_tensor, &collider_volume,
         &collision_body](const afk::physics::shape::Sphere &shape) {
          collider_inertia_tensor =
              PhysicsSystem::get_shape_inertia_tensor(shape, collision_body.mass);
          collider_volume =
              PhysicsSystem::get_shape_volume(shape, collision_body.transform.scale);
        },
        [&collider_inertia_tensor, &collider_volume,
         &collision_body](const afk::physics::shape::Box &shape) {
          collider_inertia_tensor =
              PhysicsSystem::get_shape_inertia_tensor(shape, collision_body.mass);
          collider_volume =
              PhysicsSystem::get_shape_volume(shape, collision_body.transform.scale);
        },
        [](auto &what) { afk_assert(false, "Collider shape type is invalid"); }};
    std::visit(visitor, collision_body.shape);

    // Convert the collider inertia tensor into the local-space of the body
    // do not need to worry about scale, as we are assuming that the center of mass is at the center of each collider and each collider has an even distribution of mass
    const auto collider_rotation = glm::mat3_cast(collision_body.transform.rotation);
    auto collider_rotation_transpose = glm::transpose(collider_rotation);
    // row multiplication (glm by default hhas column access)
    collider_rotation_transpose[0][0] *= collider_inertia_tensor.x;
    collider_rotation_transpose[1][0] *= collider_inertia_tensor.x;
    collider_rotation_transpose[2][0] *= collider_inertia_tensor.x;
    collider_rotation_transpose[0][1] *= collider_inertia_tensor.y;
    collider_rotation_transpose[1][1] *= collider_inertia_tensor.y;
    collider_rotation_transpose[2][1] *= collider_inertia_tensor.y;
    collider_rotation_transpose[0][2] *= collider_inertia_tensor.z;
    collider_rotation_transpose[1][2] *= collider_inertia_tensor.z;
    collider_rotation_transpose[2][2] *= collider_inertia_tensor.z;
    const auto collider_inertia_tensor_in_body_space =
        collider_rotation * collider_rotation_transpose;

    // Use the parallel axis theorem to convert the inertia tensor w.r.t the
    // collider center into a inertia tensor w.r.t to the body origin.
    // assume center of mass is the center of the collider
    // @todo convert from row-major to column-major (rp3d vs glm)
    const auto offset = collision_body.transform.translation;
    const auto radius_from_center_squared = glm::pow(glm::length(offset), 2);
    auto offset_matrix =
        glm::mat3{glm::vec3{radius_from_center_squared, 0.0f, 0.0f},
                  glm::vec3{0.0f, radius_from_center_squared, 0.0f},
                  glm::vec3{0.0f, 0.0f, radius_from_center_squared}};
    const auto offset_x = offset * (-offset.x);
    const auto offset_y = offset * (-offset.y);
    const auto offset_z = offset * (-offset.z);
    offset_matrix[0][0] *= offset_x.x;
    offset_matrix[1][0] *= offset_x.y;
    offset_matrix[2][0] *= offset_x.z;
    offset_matrix[0][1] *= offset_y.x;
    offset_matrix[1][1] *= offset_y.y;
    offset_matrix[2][1] *= offset_y.z;
    offset_matrix[0][2] *= offset_z.x;
    offset_matrix[1][2] *= offset_z.y;
    offset_matrix[2][2] *= offset_z.z;
    offset_matrix *= collision_body.mass;

    temp_inertia_tensor += collider_inertia_tensor_in_body_space + offset_matrix;
  }
  physics_component.center_of_mass /= physics_component.total_mass;

  physics_component.local_inertial_tensor =
      glm::mat3{glm::vec3{temp_inertia_tensor[0][0], 0.0f, 0.0f},
                glm::vec3{0.0f, temp_inertia_tensor[1][1], 0.0f},
                glm::vec3{0.0f, 0.0f, temp_inertia_tensor[2][2]}};
  physics_component.local_inverse_inertial_tensor =
      glm::inverse(physics_component.local_inertial_tensor);

  // change mass in static objects to have maximum values
  if (!physics_component.is_static) {
    physics_component.total_inverse_mass = 1.0f / physics_component.total_mass;
  } else {
    // set mass to largest value, set inverse to smallest value
    // set maximum values to make the object not effectively move
    physics_component.total_mass         = std::numeric_limits<f32>::max();
    physics_component.total_inverse_mass = std::numeric_limits<f32>::min();
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

          const auto &transform1 = registry.get<TransformComponent>(c.entity1);
          const auto &transform2 = registry.get<TransformComponent>(c.entity2);

          // only do resolution if at least one of the entities has a non-static physics component
          if (!physics1.is_static || !physics2.is_static) {

            afk::io::log << "collision points:\n";
            for (auto i = size_t{0}; i < c.contacts.size(); ++i) {

              // @todo add more than just the translate
              const auto world_space1 = transform1.translation + c.contacts[i].collider1_point;
              const auto world_space2 = transform2.translation + c.contacts[i].collider2_point;
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

                        // average points of collision in world space
            auto avg_collision_point1 = glm::vec3{0.0f};
            auto avg_collision_point2 = glm::vec3{0.0f};
            for (auto i = size_t{0}; i < c.contacts.size(); ++i) {
              avg_collision_point1 += c.contacts[i].collider1_point;
              avg_collision_point2 += c.contacts[i].collider2_point;
            }
            avg_collision_point1 /= c.contacts.size();
            avg_collision_point2 /= c.contacts.size();

            // vectors from center of mass to collision points
            const auto r1 = avg_collision_point1 -
                            (physics1.center_of_mass + transform1.translation);
            const auto r2 = avg_collision_point2 -
                            (physics2.center_of_mass + transform2.translation);

            const auto impulse_coefficient =
                PhysicsSystem::get_impulse_coefficient(c, avg_normal, r1, r2);

            const auto impulse = impulse_coefficient * avg_normal;

            // update forces and torque for collider 1 if it is not static
            // @todo update angular torque
            if (!physics1.is_static) {
              afk::io::log << "collision applied external forces 1\n";
              physics1.external_forces +=
                  impulse; // applying inverse mass is handled in a different function
              physics1.external_torques +=
                  impulse_coefficient * (glm::cross(r1, avg_normal));
            }
            // update forces and torque for collider 2 if it is not static
            // @todo update angular torque
            if (!physics2.is_static) {
              afk::io::log << "collision applied external forces 2\n";
              physics2.external_forces -=
                  impulse; // applying inverse mass is handled in a different function
              physics2.external_torques -=
                  impulse_coefficient * (glm::cross(r2, avg_normal));   
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
                                            const glm::vec3 &contact_normal,
                                            const glm::vec3 &r1,
                                            const glm::vec3 &r2) -> f32 {
  // @todo move this to a better place
  // 1 for fully elastic, 0 for no elastiscity at all
  const auto restitution_coefficient = 1.0f;

  auto &registry = afk::Engine::get().ecs.registry;
  auto collider1 = registry.get<ColliderComponent>(data.entity1);
  auto collider2 = registry.get<ColliderComponent>(data.entity2);

  const auto collider_1_physics = registry.get<PhysicsComponent>(data.entity1);
  const auto collider_2_physics = registry.get<PhysicsComponent>(data.entity2);

  const auto collider_1_transform = registry.get<TransformComponent>(data.entity1);
  const auto collider_2_transform = registry.get<TransformComponent>(data.entity2);

  // velocity before collision
  const auto v1 = collider_1_physics.linear_velocity;
  const auto v2 = collider_2_physics.linear_velocity;

  // rotational speed before collision
  const auto omega1 = collider_1_physics.angular_velocity;
  const auto omega2 = collider_2_physics.angular_velocity;

  // 1/(inertial tensor)
  const auto j1 = collider_1_physics.inverse_inertial_tensor;
  const auto j2 = collider_2_physics.inverse_inertial_tensor;

  auto orientation_transpose1 = glm::mat4_cast(collider_1_transform.rotation);
  auto orientation_transpose2 = glm::mat4_cast(collider_2_transform.rotation);

  // inverse mass
  // make the number as low as possible for static entities to make it appear like they're very heavy
  const auto &inverse_mass1 = collider_1_physics.total_inverse_mass;
  const auto &inverse_mass2 = collider_2_physics.total_inverse_mass;

  // -(1+e)(n + (v1 - v2) + w1.(r1 * n1) - w2.(r2 * n2))
  f32 numerator = glm::dot(contact_normal, v1 - v2);
  numerator += glm::dot(omega1, glm::cross(r1, contact_normal));
  numerator -= glm::dot(omega2, glm::cross(r2, contact_normal));
  numerator *= -(1 + restitution_coefficient);

  f32 denominator =
      glm::dot(glm::cross(r1, contact_normal), j1 * glm::cross(r1, contact_normal));
  denominator +=
      glm::dot(glm::cross(r2, contact_normal), j1 * glm::cross(r2, contact_normal));
  denominator += inverse_mass1 + inverse_mass2;

  return numerator / denominator;
}

auto PhysicsSystem::get_shape_inertia_tensor(const Sphere &shape, f32 mass) -> glm::vec3 {
  const auto single_axis_inertia = 0.4f * mass * glm::pow(shape, 2);
  return glm::vec3{single_axis_inertia, single_axis_inertia, single_axis_inertia};
}

auto PhysicsSystem::get_shape_inertia_tensor(const Box &shape, f32 mass) -> glm::vec3 {
  const auto m_over_12 = mass / 12.0f;

  // box shape is defined by half extents, need to find full widths
  const auto x2 = shape.x * 2.0f;
  const auto y2 = shape.y * 2.0f;
  const auto z2 = shape.z * 2.0f;
  return glm::vec3{m_over_12 * (glm::pow(y2, 2) + glm::pow(z2, 2)),
                   m_over_12 * (glm::pow(x2, 2) + glm::pow(z2, 2)),
                   m_over_12 * (glm::pow(x2, 2) + glm::pow(y2, 2))};
}

auto PhysicsSystem::get_shape_volume(const Sphere &shape, const glm::vec3 &scale) -> f32 {
  // for a sphere to be a sphere, its radius needs to be consistent
  const auto avg_radius = ((scale.x + scale.y + scale.z) / 3.0f) * shape;
  return (4.0f / 3.0f) * glm::pi<f32>() * glm::pow(avg_radius, 3);
}

auto PhysicsSystem::get_shape_volume(const Box &shape, const glm::vec3 &scale) -> f32 {
  // box shape is defined by half extents, so they need to be converted to full extents
  return (shape.x * 2.0f * scale.x) * (shape.y * 2.0f * scale.y) *
         (shape.z * 2.0f * scale.z);
}
