#include "afk/physics/PhysicsBodySystem.hpp"

#include "afk/Afk.hpp"

using afk::physics::PhysicsBodySystem;
using afk::physics::Transform;

PhysicsBodySystem::PhysicsBodySystem() {
  this->world->setIsGravityEnabled(false);
  this->world->setEventListener(&listener);
}

auto PhysicsBodySystem::update(entt::registry *registry, float dt) -> void {
  this->world->update(dt);

  // todo: update reactphysics world with where the object changes position in the game world
}

void PhysicsBodySystem::CollisionEventListener::onContact(
    const rp3d::CollisionCallback::CallbackData &callback_data) {
  // For each contact pair
  for (rp3d::uint p = 0; p < callback_data.getNbContactPairs(); p++) {

    // Get the contact pair
    CollisionCallback::ContactPair contactPair = callback_data.getContactPair(p);

    // find colliding entities in AFK's ECS
    const auto &body1_id = contactPair.getBody1()->getEntity().id;
    const auto &body2_id = contactPair.getBody2()->getEntity().id;

    afk::GameObject *object1 = nullptr;
    afk::GameObject *object2 = nullptr;

    auto physics_view =
        afk::Engine::get().registry.view<PhysicsBody>();
    for (auto &entity : physics_view) {
      auto &physics_component = physics_view.get<PhysicsBody>(entity);
      if (physics_component.body->getEntity().id == body1_id) {
        object1 = const_cast<afk::GameObject *>(&entity);
      } else if (physics_component.body->getEntity().id == body2_id) {
        object2 = const_cast<afk::GameObject *>(&entity);
      }
      if ((object1 != nullptr) && (object2 != nullptr)) {
        break;
      }
    }

    if ((object1 != nullptr) && (object2 != nullptr)) {
      
      if (contactPair.getEventType() == CollisionCallback::ContactPair::EventType::ContactStart) {
        std::vector<glm::vec3> contact_points = {};
        contact_points.reserve(contactPair.getNbContactPoints());
        for (u32 i = 0; i < contactPair.getNbContactPoints(); ++i) {
          auto contact_point = contactPair.getContactPoint(i).getWorldNormal();
          contact_points.emplace_back(contact_point.x, contact_point.y, contact_point.z);
        }
        afk::Engine::get().physics_body_system.collision_enter_queue.emplace(PhysicsBodySystem::Collision{
            *object1,
            *object2,
            std::move(contact_points)
        });
      } else if (contactPair.getEventType() ==
                 CollisionCallback::ContactPair::EventType::ContactExit) {
        std::vector<glm::vec3> contact_points = {};
        contact_points.reserve(contactPair.getNbContactPoints());
        for (u32 i = 0; i < contactPair.getNbContactPoints(); ++i) {
          auto contact_point = contactPair.getContactPoint(i).getWorldNormal();
          contact_points.emplace_back(contact_point.x, contact_point.y, contact_point.z);
        }
        afk::Engine::get().physics_body_system.collision_enter_queue.emplace(PhysicsBodySystem::Collision{
            *object1,
            *object2,
            std::move(contact_points)
        });
      } else {
        // ContactStay may not be triggered if rigid body is sleeping
        
      }
    }
  }
}