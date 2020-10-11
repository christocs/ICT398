#pragma once

#include <memory>
#include <variant>
#include <vector>

#include <entt/entt.hpp>
#include <reactphysics3d/reactphysics3d.h>

#include "afk/component/BaseComponent.hpp"
#include "afk/physics/PhysicsBodySystem.hpp"
#include "afk/physics/Transform.hpp"
#include "afk/physics/shape/Box.hpp"
#include "afk/physics/shape/Capsule.hpp"
#include "afk/physics/shape/Sphere.hpp"
#include "glm/vec3.hpp"

namespace afk {
  namespace physics {
    class PhysicsBodySystem;

    /** Types of collider shapes that could be used as a collider */
    using CollisionBodyVariant =
        std::variant<afk::physics::shape::Box, afk::physics::shape::Sphere, afk::physics::shape::Capsule>;

    /** store the collision body type from above, this should be unused now */
    enum class CollisionBodyType { Box, Sphere, Capsule };

    /** static bodies never move, dynamic bodies move according to interactions with static and dynamic bodies + gravity */
    enum class BodyType { Static, Dynamic };

    /** representation of a single collider for a group of colliders */
    struct CollisionBody {
      CollisionBodyType type = {}; // todo: remove need for this by using std::visit
      CollisionBodyVariant body         = {};
      afk::physics::Transform transform = {}; // transform relative to parent
    };

    /** group of colliders to use in a single physics body */
    using CollisionBodyCollection = std::vector<CollisionBody>;

    class PhysicsBody : public afk::BaseComponent {
    public:
      PhysicsBody() = delete;

      /**
       * constructor
       * instantiates the physics body into the rp3d world
       */
      PhysicsBody(entt::entity body, PhysicsBodySystem *physics_system,
                  const afk::physics::Transform &transform,
                  const CollisionBodyCollection &collision_bodies, BodyType type);

      // todo add rotate method

      /**
       * translate the collision body in rp3d (but not in afk ecs)
       * @param translate
       */
      void translate(glm::vec3 translate);

      /**
       * Set position of the collision body in rp3d (but not in afk ecs)
       * @param pos
       */
      void set_pos(glm::vec3 pos);

    private:
      /**
       * create rp3d box shape
       * note that it takes scale into account as rp3d's transform does not include scale, so scaling needs to be doing when creating shapes
       * @param shape
       * @param scale
       * @return
       */
      static rp3d::BoxShape *createShapeBox(const afk::physics::shape::Box &shape,
                                            const glm::vec3 &scale);

      /**
       * create rp3d sphere shape
       * note that it takes scale into account as rp3d's transform does not include scale, so scaling needs to be doing when creating shapes
       * @param shape
       * @param scale
       * @return
       */
      static rp3d::SphereShape *createShapeSphere(const afk::physics::shape::Sphere &shape,
                                                  const glm::vec3 &scale);

      /**
       * create rp3d capsule shape
       * note that it takes scale into account as rp3d's transform does not include scale, so scaling needs to be doing when creating shapes
       * @param shape
       * @param scale
       * @return
       */
      static rp3d::CapsuleShape *createShapeCapsule(const afk::physics::shape::Capsule &shape,
                                                    const glm::vec3 &scale);

      /**
       * rp3d representation of the collision body
       */
      rp3d::CollisionBody *body = nullptr;

      /**
       * body type (static, dynamic)
       */
      BodyType type = {};

      /**
       * Current velocity of the physics body
       */
      glm::vec3 velocity = {0.0f, 0.0f, 0.0f};

      friend class PhysicsBodySystem;
    };
  }
}
