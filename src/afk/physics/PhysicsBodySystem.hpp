#pragma once

#include <memory>
#include <queue>
#include <vector>

#include <entt/entt.hpp>
#include <reactphysics3d/reactphysics3d.h>

#include "afk/NumericTypes.hpp"
#include "afk/physics/PhysicsBody.hpp"
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

      auto update(entt::registry *registry, float dt) -> void;

    private:
      class CollisionEventListener : public rp3d::EventListener {
        virtual void onContact(const rp3d::CollisionCallback::CallbackData &callback_data) override;
      };

      struct Collision {
        GameObject entity1                    = {};
        GameObject entity2                    = {};
        std::vector<glm::vec3> contact_points = {};
      };

      std::queue<Collision> collision_enter_queue = {};
      std::queue<Collision> collision_exit_queue  = {};

      rp3d::PhysicsCommon physics_common = {};
      rp3d::PhysicsWorld *world          = physics_common.createPhysicsWorld();
      CollisionEventListener listener    = {};

      friend class PhysicsBody;
    };
  }
}
