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
        auto initialize_physics_component(afk::ecs::component::PhysicsComponent &physics_component,
                                           const afk::ecs::component::ColliderComponent &collider_component)
            -> void;

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

        static auto get_impulse_coefficient(const afk::event::Event::Collision &data,
                                            const glm::vec3 &contact_normal,
                                            const glm::vec3 &r1,
                                            const glm::vec3 &r2) -> f32;

      private:
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

        /** Is the physics system initialized? */
        bool is_initialized = false;
      };
    }
  }
}
