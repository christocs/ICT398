#pragma once

#include <glm/glm.hpp>

#include "afk/ecs/component/ColliderComponent.hpp";
#include "afk/ecs/component/PhysicsComponent.hpp"
#include "afk/ecs/component/TransformComponent.hpp";
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
         * Load a physics component associated to an entity
         */
        auto instantiate_physics_component(const afk::ecs::Entity &entity,
                                           afk::ecs::component::PhysicsComponent &physics_component,
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
                                            const glm::vec3 &contact_normal) -> f32;

      private:
        /** Is the physics system initialized? */
        bool is_initialized = false;
      };
    }
  }
}
