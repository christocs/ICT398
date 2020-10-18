#pragma once

#include <unordered_map>

#include <reactphysics3d/reactphysics3d.h>

#include "afk/ecs/Entity.hpp"
#include "afk/ecs/component/ColliderComponent.hpp"
#include "afk/render/Mesh.hpp"

namespace afk {
  namespace ecs {
    namespace system {
      class CollisionSystem {
      public:
        /**
         * Constructor
         *
         * @todo test this works
         * @todo remove the need to turn off the sleeping optimisation in ReactPhysics3D
         * @todo turn off debug ReactPhysics3D data to be generated in ReactPhysics3D when debug rendering is not being used
         * @todo set which debug items to generate display data for in GUI
         */
        CollisionSystem();

        /**
         * Destructor
         *
         * ReactPhysics3D memory is self-managed and does not need to be manually destroyed
         * Once the instance of rp3d::PhysicsCommon is destroyed it will deallocate all memory it has assigned
         */
        ~CollisionSystem() = default;

        CollisionSystem(CollisionSystem &&)      = delete;
        CollisionSystem(const CollisionSystem &) = delete;
        auto operator=(const CollisionSystem &) -> CollisionSystem & = delete;
        auto operator=(CollisionSystem &&) -> CollisionSystem & = delete;

        /** Update collisions */
        auto update() -> void;

        /**
         * Load a collision component associated to an entity
         *
         * @todo instead of creating new shapes for each entity, check if the prefab has already been instantiated and use shapes from the previous instantiation
         * @todo apply parent transformation to each collider
         */
        auto instantiate_collider(const afk::ecs::Entity &entity,
                           const afk::ecs::component::ColliderComponent &collider_component)
            -> void;

        /**
         * Hack to get debug collision data out of reactphysics3d
         *
         * @return
         *
         * @todo remove this
         */
        auto get_debug_mesh() -> afk::render::Mesh;

      private:
        /** alias to ReactPhysics3D ids for their internal rp3d ECS */
        using rp3d_id = rp3d::uint;

        /** Logger class for logging ReactPhysics3D events
         * @todo Enable/disable logs by level in GUI
         * @todo Be able to display logs separate from game logs in GUI
         */
        class Logger : public rp3d::Logger {
          void log(rp3d::Logger::Level level, const std::string &physicsWorldName,
                   rp3d::Logger::Category category, const std::string &message,
                   const char *filename, int lineNumber);
        };

        /**
         * Event listener class for when collisions occur in ReactPhysics3D
         * Will fire on each collision event during an update calll to the rp3d world
         */
        class CollisionEventListener : public rp3d::EventListener {
          virtual void onContact(const rp3d::CollisionCallback::CallbackData &callback_data) override;
        };

        /**
         * Create a ReactPhysics3D box shape
         *
         * @param shape the box shape
         * @param scale scale of the shape
         */
        rp3d::BoxShape *create_shape_box(const afk::physics::shape::Box &shape,
                                         const glm::vec3 &scale);

        /**
         * Create a ReactPhysics3D sphere shape
         *
         * Scale will scale every axis equally, the average scale will be used to determine the scale
         * This is a limitation of reactphysics3d, otherwise the shape wouldn't be a 'sphere'
         *
         * @param shape the sphere shape
         * @param scale scale of the shape
         */
        rp3d::SphereShape *create_shape_sphere(const afk::physics::shape::Sphere &shape,
                                               const glm::vec3 &scale);

        /**
         * Create a ReactPhysics3D capsule shape
         *
         * @param shape the capsule shape
         * @param scale scale of the shape
         */
        rp3d::CapsuleShape *create_shape_capsule(const afk::physics::shape::Capsule &shape,
                                                 const glm::vec3 &scale);

        /** Event listener used for firing collision events that occur in the ReactPhysics3D world */
        CollisionEventListener event_listener = {};

        /** Logger used for displaying ReactPhysics3D events */
        Logger logger = {};

        /** ReactPhysics3D library resource manager */
        rp3d::PhysicsCommon physics_common = {};

        /** ReactPhysics3D representation of the world */
        rp3d::PhysicsWorld *world = nullptr;

        /** Map to point the AFK ECS entity to the ReactPhysics3D collision body identifier */
        std::unordered_map<ecs::Entity, rp3d_id> ecs_entity_to_rp3d_body_map = {};

        /** Map to point the ReactPhysics3D collision body identifier an AFK ECS entity */
        std::unordered_map<rp3d_id, ecs::Entity> rp3d_body_to_ecs_entity_map = {};
      };
    }
  }
};
