#pragma once

#include <memory>
#include <queue>
#include <vector>

#include <entt/entt.hpp>
#include <reactphysics3d/reactphysics3d.h>

#include "afk/NumericTypes.hpp"
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
      PhysicsBodySystem();

      auto update(float dt) -> void;

      auto get_debug_model() -> afk::render::Model;

    private:
      class CollisionEventListener : public rp3d::EventListener {
        virtual void onContact(const rp3d::CollisionCallback::CallbackData &callback_data) override;
      };

      class Logger : public rp3d::Logger {
        void log(Level level, const std::string &physicsWorldName, Category category,
                 const std::string &message, const char *filename, int lineNumber);
      };

      // todo: give better names
      auto set_debug_physics_item(const afk::render::debug::PhysicsView &physics_view,
                                  bool status) -> void;
      auto get_debug_physics_item(const afk::render::debug::PhysicsView &physics_view) const
          -> bool;

      auto get_debug_mesh() -> afk::render::Mesh;

      rp3d::PhysicsCommon physics_common                            = {};
      rp3d::PhysicsWorld *world                                     = nullptr;
      CollisionEventListener listener                               = {};
      afk::render::Model model                                      = {};
      std::unordered_map<u32, afk::GameObject> rp3d_body_to_ecs_map = {};
      Logger logger                                                 = {};
      glm::vec3 gravity = {0.0f, -9.81f, 0.0f};

      friend class PhysicsBody;
      friend afk::render::Renderer;
    };
  }
}
