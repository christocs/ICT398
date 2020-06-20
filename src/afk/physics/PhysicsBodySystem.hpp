#pragma once

#include <memory>

#include <entt/entt.hpp>
#include <reactphysics3d/reactphysics3d.h>

#include "afk/physics/PhysicsBody.hpp"
#include "glm/vec3.hpp"

namespace Afk {
  class PhysicsBody;
  /**
   * Physics body management
   */
  class PhysicsBodySystem {
  public:
    PhysicsBodySystem() = delete;

    PhysicsBodySystem(glm::vec3 gravity);

    auto get_gravity();

    auto set_gravity(glm::vec3 gravity);

    auto update(entt::registry *registry, float dt) -> void;

  private:
    rp3d::PhysicsCommon physics_common = {};
    rp3d::PhysicsWorld *world          = physics_common.createPhysicsWorld();

    friend class PhysicsBody;
  };
};
