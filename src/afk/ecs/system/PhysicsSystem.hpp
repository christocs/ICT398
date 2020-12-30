#pragma once

#include <glm/glm.hpp>

#include "afk/ecs/component/ColliderComponent.hpp"
#include "afk/ecs/component/PhysicsComponent.hpp"
#include "afk/ecs/component/TransformComponent.hpp"
#include "afk/event/Event.hpp"
#include "afk/physics/Transform.hpp"
#include "afk/physics/shape/Box.hpp"
#include "afk/physics/shape/Sphere.hpp"

namespace afk {
  namespace ecs {
    namespace system {
      /**
       * Handles physics resolution for entities with PhysicsComponent
       */
      class PhysicsSystem {
      public:
        /** Initialise the physics system */
        auto initialize() -> void;

        /**
         * Update physics resolution
         */
        auto update() -> void;

        /**
         * Initialize values for physics component
         *
         * @param physics_component physics component to initialise its data
         * @param collider_component collider component used to generate the physics component's data
         * @param transform_component transform component used to generate the phhysics component's data
         */
        auto initialize_physics_component(
            afk::ecs::component::PhysicsComponent &physics_component,
            const afk::ecs::component::ColliderComponent &collider_component,
            const afk::ecs::component::TransformComponent &transform_component) -> void;

        /**
         * Callback to call when a collision occurs
         *
         * Should only pass in event of type Collision
         *
         * @param event event information
         *
         * @todo copy event parameter by reference not value
         */
        static auto collision_resolution_callback(afk::event::Event event) -> void;

      private:
        /**
         * Apply changes queued for rigid bodies
         */
        auto apply_rigid_body_changes(f32 dt) -> void;

        /**
         * Method depenetrates non-static rigid bodies from other colliders
         * May cause new, different penetrations so it is recommende to run this multiple times
         *
         * Does NOT re-syncronise the transform components and the colliders, so be sure to syncronise them after calling this
         *
         * @return number of rigid bodies de-penetrated
         */
        auto depenetrate_dynamic_rigid_bodies() -> u32;

        /**
         * Get the coefficient of the impulse vector
         *
         * @param data collision event data
         * @param contact_normal the contact normal from the first body to the second
         * @param r1 the first body's vector from center of mass to its collision point in local space
         * @param r2 the second body's vector from center of mass to its collision point in local space
         */
        static auto get_impulse_coefficient(const afk::event::Event::Collision &data,
                                            const glm::vec3 &contact_normal,
                                            const glm::vec3 &r1,
                                            const glm::vec3 &r2) -> f32;

        /**
         * Get inertia tensor of a sphere shape in its own local space
         *
         * @param shape definition of the individual collider
         * @param mass mass of the individual collider
         *
         * @return inertia tensor in local space
         *
         * @todo reference source of equation
         */
        static auto get_shape_inertia_tensor(const afk::physics::shape::Sphere &shape,
                                             f32 mass) -> glm::vec3;

        /**
         * Get inertia tensor of a box shape in its own local space
         *
         * @param shape definition of the individual collider
         * @param mass mass of the individual collider
         *
         * @return inertia tensor in local space
         *
         * @todo reference source of equation
         */
        static auto get_shape_inertia_tensor(const afk::physics::shape::Box &shape,
                                             f32 mass) -> glm::vec3;

        /**
         * Get volume of sphere shape within the rigid body's local space
         *
         * @param shape definition of the individual collider
         * @param scale scale of the individual collider
         *
         * @return volume of the shape with scale applied
         */
        static auto get_shape_volume(const afk::physics::shape::Sphere &shape,
                                     const glm::vec3 &scale) -> f32;

        /**
         * Get volume of box shape within the rigid body's local space
         *
         * @param shape definition of the individual collider
         * @param scale scale of the individual collider
         *
         * @return volume of the shape with scale applied
         */
        static auto get_shape_volume(const afk::physics::shape::Box &shape,
                                     const glm::vec3 &scale) -> f32;

        /**
         * Get the local center of mass
         *
         * @param collider_component collider component to generate the centre off mass for
         * @param total_mass total mass of the collider component
         *
         * @return center of mass of the collider component in its local space
         */
        static auto get_local_center_of_mass(const afk::ecs::component::ColliderComponent &collider_component,
                                             f32 total_mass) -> glm::vec3;

        /**
         * Get total mass
         *
         * @param collider_component collider component to calculate the total mass for
         *
         * @return total mass of the collider
         */
        static auto get_total_mass(const afk::ecs::component::ColliderComponent &collider_component)
            -> f32;

        /**
         * Calculate inertia tensor in a collider component's local space
         *
         * @param collider_component the collider component to calculate the inertia tensor for
         * @param local_center_of_mass center of mass in local space of the collider component
         */
        static auto get_local_inertia_tensor(const afk::ecs::component::ColliderComponent &collider_component,
                                             const glm::vec3 &local_center_of_mass)
            -> glm::vec3;

        /**
         * Calcualate the inverse inertia tensor in global space using the rotation for the entity
         *
         * @param local_inverse_inertia_tensor inertia tensor of the collider component in local space
         * @param rotation the global rotation of the collider component
         */
        static auto get_inverse_inertia_tensor(const glm::vec3 &local_inverse_inertia_tensor,
                                               const glm::quat &rotation) -> glm::mat3;

        /** Is the physics system initialized? */
        bool is_initialized = false;

        /** maximum number of times to run depenetration per update */
        static constexpr u32 DEPENETRATION_MAXIMUM_ITERATIONS = 10;

        /** maximum penetration value */
        static constexpr f32 MAXIMUM_PENETRATION = 0.1f;

        /**
         * coefficient of restitution
         * 1 for perfectly elastic collisions, 0 for all energy lost
         */
        static constexpr f32 COEFFICENT_OF_RESTITUTION = 0.9f;
      };
    }
  }
}
