#pragma once

#include <memory>

#include <entt/entt.hpp>
#include <reactphysics3d/reactphysics3d.h>

#include "afk/physics/PhysicsBody.hpp"
#include "glm/vec3.hpp"

namespace Afk {
  class PhysicsBody;

  class PhysicsBodySystem {
  public:
    PhysicsBodySystem() = delete;

    PhysicsBodySystem(glm::vec3 gravity);

    auto get_gravity();

    auto set_gravity(glm::vec3 gravity);

    auto update(entt::registry *registry, float dt) -> void;

  private:
    class CollisionEventListener : public rp3d::EventListener {
      virtual void onContact(const rp3d::CollisionCallback::CallbackData &callback_data) override;
    };

    rp3d::PhysicsCommon physics_common = {};
    rp3d::PhysicsWorld *world          = physics_common.createPhysicsWorld();
    CollisionEventListener listener    = {};

    friend class PhysicsBody;
  };
};
