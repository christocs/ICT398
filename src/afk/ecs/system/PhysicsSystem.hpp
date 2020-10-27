#pragma once

#include <glm/glm.hpp>

#include "afk/ecs/component/ColliderComponent.hpp";
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
         * @param event - event information
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
         */
        static auto get_impulse_coefficient(const afk::event::Event::Collision &data,
                                            const glm::vec3 &contact_normal,
                                            const glm::vec3 &r1,
                                            const glm::vec3 &r2) -> f32;

        /**
         * Get inertia tensor of a sphere shape in its own local space
         *
         * @param shape
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
         * @param shape
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
         * @param shape
         * @param scale
         *
         * @return volume
         */
        static auto get_shape_volume(const afk::physics::shape::Sphere &shape,
                                     const glm::vec3 &scale) -> f32;

        /**
         * Get volume of box shape within the rigid body's local space
         *
         * @param shape
         * @param scale
         *
         * @return volume
         */
        static auto get_shape_volume(const afk::physics::shape::Box &shape,
                                     const glm::vec3 &scale) -> f32;

        /**
         * Calculate local center of mass
         */
        static auto get_local_center_of_mass(const afk::ecs::component::ColliderComponent &collider_component,
                                             f32 total_mass) -> glm::vec3;

        /**
         * Calculate total mass
         */
        static auto get_total_mass(const afk::ecs::component::ColliderComponent &collider_component)
            -> f32;

        /**
         * Calculate inertia tensor in a collider's local space
         */
        static auto get_local_inertia_tensor(const afk::ecs::component::ColliderComponent &collider_component,
                                             f32 total_mass, const glm::vec3 &local_center_of_mass)
            -> glm::vec3;

        /**
         * Calcualte thhe inverse inertia tensor in global space using the rotation for the entity
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
