#pragma once

#include <memory>
#include <queue>
#include <vector>

#include <entt/entt.hpp>
#include <reactphysics3d/reactphysics3d.h>

#include "afk/NumericTypes.hpp"
#include "afk/event/Event.hpp"
#include "afk/physics/PhysicsBody.hpp"
#include "afk/render/Debug.hpp"
#include "afk/render/Renderer.hpp"
#include "glm/vec3.hpp"

namespace afk {
  namespace physics {
    class PhysicsBody;
    /**
     * Physics body management
     */
    class PhysicsBodySystem {
    public:
      /**
       * Constructor
       *
       * Initialises physics world without any colliders
       * Enables debug information
       */
      PhysicsBodySystem();

      /**
       * Destructor
       *
       * Unecessarily destroys the physics world, this is not necessary as the instance of rp3d::PhysicsCommon will automatically deallocate any memory it has allocated including thee physics world
       */
      ~PhysicsBodySystem();

      /**
       * Update the physics system
       *
       * Applies gravity to all non-static PhysicsBodys
       * Synchronises the AFK Transform component and the rp3d Transform data
       * Note that it does not sync scale data, as reactphysics3d's transform does not have a scale
       * @param dt
       */
      auto update(float dt) -> void;

      /**
       * Unimplemented physics resolution for handling collision impulse events
       * @param data
       */
      void resolve_collision_event(const afk::event::Event::CollisionImpulse &data);

      /** Get debug model from debug renderer */
      auto get_debug_model() -> afk::render::Model;

    private:
      /**
       * Collision callback class that fires collision events
       * This collision callback is used when manually testing collisions
       */
      class CollisionCallback : public rp3d::CollisionCallback {
        virtual void onContact(const rp3d::CollisionCallback::CallbackData &callback_data);
      };

      /** Logger class for logging ReactPhysics3D events
       * @todo Enable/disable logs by level in GUI
       * @todo Be able to display logs separate from game logs in GUI
       */
      class Logger : public rp3d::Logger {
        void log(Level level, const std::string &physicsWorldName, Category category,
                 const std::string &message, const char *filename, int lineNumber);
      };

      /**
       * Set which rp3d debug render item to view (e.g. contact points)
       * @param physics_view
       * @param status
       */
      auto set_debug_physics_item(const afk::render::debug::PhysicsView &physics_view,
                                  bool status) -> void;

      /**
       * Get wheather a rp3d debug renderer is set or not (e.g. contact points)
       * @param physics_view
       * @return
       */
      auto get_debug_physics_item(const afk::render::debug::PhysicsView &physics_view) const
          -> bool;

      /**
       * Get the debug mesh with data generated in rp3d
       * Note that the data generated internally in rp3d for this only updates when this->world->update(dt) is called
       * @return
       */
      auto get_debug_mesh() -> afk::render::Mesh;

      /**
       * Resource allocator for rp3d
       */
      rp3d::PhysicsCommon physics_common   = {};
      /**
       * rp3d world
       */
      rp3d::PhysicsWorld *world            = nullptr;
      /**
       * Callback to be called when doing collision tests
       */
      CollisionCallback collision_callback = {};
      /**
       * debug renderer model
       */
      afk::render::Model model             = {};
      /**
       * map between the rp3d body id and the entt entity id
       */
      std::unordered_map<rp3d::uint, entt::entity> rp3d_body_to_ecs_map = {};
      /**
       * cli logger for logging rp3d world events
       */
      Logger logger                                                     = {};
      /**
       * gravity to be applied on each update
       */
      glm::vec3 gravity = {0.0f, -9.81f, 0.0f};

      /**
       * Getting a little too friendly here for my liking
       */
      friend class PhysicsBody;
      friend afk::render::Renderer;
    };
  }
}
