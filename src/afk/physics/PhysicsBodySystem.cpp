#include "afk/physics/PhysicsBodySystem.hpp"

#include "afk/afk.hpp"
#include "afk/component/TagComponent.hpp"

using Afk::PhysicsBodySystem;

PhysicsBodySystem::PhysicsBodySystem(glm::vec3 gravity) {
  auto g = rp3d::Vector3(gravity.x, gravity.y, gravity.z);
  this->world->setGravity(g);
  this->world->setEventListener(&listener);
}

auto PhysicsBodySystem::get_gravity() {
  return glm::vec3{this->world->getGravity().x, this->world->getGravity().y,
                   this->world->getGravity().z};
}

auto PhysicsBodySystem::set_gravity(glm::vec3 gravity) {
  auto g = rp3d::Vector3(gravity.x, gravity.y, gravity.z);
  this->world->setGravity(g);
}

auto PhysicsBodySystem::update(entt::registry *registry, float dt) -> void {
  this->world->update(dt);

  // Mirror changes in physics engine to Transform component
  // TODO: Scale shapes of rigid bodies on the fly, updates in v0.8.0 might help with this
  // @see https://github.com/DanielChappuis/reactphysics3d/issues/103
  registry->view<Afk::Transform, Afk::PhysicsBody>().each(
      [](Afk::Transform &transform, Afk::PhysicsBody &collision) {
        const auto &rp3d_position = collision.body->getTransform().getPosition();
        const auto &rp3d_orientation = collision.body->getTransform().getOrientation();

        transform.translation =
            glm::vec3{rp3d_position.x, rp3d_position.y, rp3d_position.z};
        transform.rotation = glm::quat{rp3d_orientation.w, rp3d_orientation.x,
                                       rp3d_orientation.y, rp3d_orientation.z};
      });
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

    GameObject *object1 = nullptr;
    GameObject *object2 = nullptr;

    // only search traverse items with tags, as they determine what type of collision the event is
    auto physics_view =
        Afk::Engine::get().registry.view<Afk::PhysicsBody, Afk::TagComponent>();
    for (auto &entity : physics_view) {
      auto &physics_component = physics_view.get<Afk::PhysicsBody>(entity);
      if (physics_component.body->getEntity().id == body1_id) {
        object1 = const_cast<GameObject *>(&entity);
      } else if (physics_component.body->getEntity().id == body2_id) {
        object2 = const_cast<GameObject *>(&entity);
      }
      if ((object1 != nullptr) && (object2 != nullptr)) {
        break;
      }
    }

    if ((object1 != nullptr) && (object2 != nullptr)) {
      Event::CollisionAction collision_action{false, false, false};
      if (contactPair.getEventType() == CollisionCallback::ContactPair::EventType::ContactStart) {
        collision_action.contact_start = true;
      } else if (contactPair.getEventType() ==
                 CollisionCallback::ContactPair::EventType::ContactExit) {
        collision_action.contact_end = true;
      } else {
        // ContactStay may not be triggered if rigid body is sleeping
        collision_action.contact_stay = true;
      }

      auto &object1_tags =
          Afk::Engine::get().registry.get<Afk::TagComponent>(*object1).tags;
      auto &object2_tags =
          Afk::Engine::get().registry.get<Afk::TagComponent>(*object2).tags;

      // only try find interactions between the player and other objects, as that is all the event system handles
      TagComponent::Tags *player_object_tags = nullptr;
      TagComponent::Tags *other_object_tags  = nullptr;
      if (object1_tags.count(Afk::TagComponent::PLAYER) == 1) {
        player_object_tags = &object1_tags;
        other_object_tags  = &object2_tags;
      } else if (object2_tags.count(Afk::TagComponent::PLAYER) == 1) {
        other_object_tags  = &object1_tags;
        player_object_tags = &object2_tags;
      }

      // other object tag will never be a nullptr if the player tag is found
      if (player_object_tags != nullptr) {
//        std::cout << "contact player" << std::endl;
        Event::Collision collision_event_data{Event::CollisionType{false, false, false}, collision_action};
        if (other_object_tags->count(Afk::TagComponent::ENEMY) == 1) {
          collision_event_data.type.enemy = 1;
        }

        if (other_object_tags->count(Afk::TagComponent::PREY)  == 1) {
          collision_event_data.type.prey = 1;
        }

        if (other_object_tags->count(Afk::TagComponent::DEATHZONE) == 1) {
          collision_event_data.type.deathzone = 1;
        }

        Event event;
        event.data = collision_event_data;
        event.type = Event::Type::Collision;
        Afk::Engine::get().event_manager.queue_event(event);
      }
    }
  }
}