#include "afk/ecs/system/CollisionSystem.hpp"

#include "afk/Engine.hpp"
#include "afk/debug/Assert.hpp"
#include "afk/ecs/component/PhysicsComponent.hpp"
#include "afk/io/Log.hpp"
#include "afk/utility/Visitor.hpp"

using afk::ecs::component::ColliderComponent;
using afk::ecs::component::TransformComponent;
using afk::ecs::component::PhysicsComponent;
using afk::ecs::system::CollisionSystem;

CollisionSystem::CollisionSystem() {
  // Instantiate the world
  this->world = this->physics_common.createPhysicsWorld();
  // Disable gravity; ReactPhysics3D will only be used for collision detection, not applying physics
  this->world->setIsGravityEnabled(false);

  // Turn off sleeping of collisions.
  // Then bodies are colliding but aren't really changing their state for a while, they will be put to 'sleep' to stop testing collisions.
  // Putting a body to sleep will make it no longer appear to be colliding externally.
  // Temporarily, we will be disabling this optimisation for simplicity's sake, so we can process every collision
  // @todo remove the need to turn off the sleeping optimisation in ReactPhysics3D
  this->world->enableSleeping(false);

  // Set event listener used for firing collision events that occur in the ReactPhysics3D world
  this->world->setEventListener(&this->event_listener);

  // turn on debug renderer so ReactPhysics3D creates render data
  // @todo turn off debug ReactPhysics3D data to be generated in ReactPhysics3D when debug rendering is not being used
  this->world->setIsDebugRenderingEnabled(true);

  // Set all debug items to be displayed
  // @todo set which debug items to generate display data for in GUI
  this->world->getDebugRenderer().setIsDebugItemDisplayed(
      rp3d::DebugRenderer::DebugItem::COLLIDER_AABB, true);
  this->world->getDebugRenderer().setIsDebugItemDisplayed(
      rp3d::DebugRenderer::DebugItem::COLLIDER_BROADPHASE_AABB, true);
  this->world->getDebugRenderer().setIsDebugItemDisplayed(
      rp3d::DebugRenderer::DebugItem::COLLISION_SHAPE, true);
  this->world->getDebugRenderer().setIsDebugItemDisplayed(
      rp3d::DebugRenderer::DebugItem::CONTACT_POINT, true);
  this->world->getDebugRenderer().setIsDebugItemDisplayed(
      rp3d::DebugRenderer::DebugItem::CONTACT_NORMAL, true);

  // Set the debug logger for events in ReactPhysics3D
  this->physics_common.setLogger(&(this->logger));
}

auto CollisionSystem::update() -> void {
  auto registry = &afk::Engine::get().ecs.registry;
  // not having a PhysicsComponent means the entity is 'static', so ones with a PhysicsComponent are 'dynamic'
  // so here we are only updating rigid bodies that are dynamic
  auto collider_view =
      registry->view<ColliderComponent, TransformComponent, PhysicsComponent>();

  // update translation and rotation in physics world
  // @todo find how to apply scale dynamically, most likely need to trigger a change and at that point make new rp3d shapes that are scaled
  for (auto &entity : collider_view) {
    const auto &collider =
        collider_view.get<ColliderComponent>(entity);
    const auto &transform =
        collider_view.get<TransformComponent>(entity);
    afk_assert(this->ecs_entity_to_rp3d_body_index_map.count(entity) == 1,
               "ECS entity is not mapped to a rp3d body");
    const auto rp3d_body_index = this->ecs_entity_to_rp3d_body_index_map.at(entity);
    const auto rp3d_body       = this->world->getCollisionBody(rp3d_body_index);

    const auto rp3d_transform = rp3d::Transform(
        rp3d::Vector3(transform.translation.x, transform.translation.y,
                      transform.translation.z),
        rp3d::Quaternion(transform.rotation.x, transform.rotation.y,
                         transform.rotation.z, transform.rotation.w));

    rp3d_body->setTransform(rp3d_transform);
  }

  // update React3DPhysics world
  // this method calls to update the debug render data
  // this method fires collision events
  // this method also unnecessarily does physics calculations for any rigid bodies, though none should be created in the game engine
  this->world->update(afk::Engine::get().get_delta_time());
}

auto CollisionSystem::instantiate_collider(
    const afk::ecs::Entity &entity,
    const afk::ecs::component::ColliderComponent &collider_component,
    const afk::ecs::component::TransformComponent &transform_component) -> void {
  // check if entity has already had a collider component loaded
  afk_assert(
      this->ecs_entity_to_rp3d_body_index_map.count(entity) == 0,
      "Collider component has already being loaded for the given entity");

  const auto rp3d_parent_transform =
      rp3d::Transform(rp3d::Vector3(transform_component.translation.x,
                                    transform_component.translation.y,
                                    transform_component.translation.z),
                      rp3d::Quaternion(transform_component.rotation.x,
                                       transform_component.rotation.y,
                                       transform_component.rotation.z,
                                       transform_component.rotation.w));

  // instantiate rp3d body, applying the appropriate translation and rotation
  // note that the scale is not included in rp3d transform, so collision bodies will be manually scaled later
  auto body = this->world->createCollisionBody(rp3d_parent_transform);

  // find the index of the collision body
  const auto no_collision_bodies = this->world->getNbCollisionBodies();
  for (auto i = size_t{0}; i < no_collision_bodies; ++i) {
    if (this->world->getCollisionBody(i) == body) {
      // check that the rp3d body has not already been mapped to a afk ecs entity
      afk_assert(this->rp3d_body_index_to_ecs_entity_map.count(i) == 0,
                 "ReactPhysics3D body index has already being mapped to an AFK "
                 "ECS entity");

      // add associations between rp3d body id and afk ecs entity
      this->ecs_entity_to_rp3d_body_index_map.insert({entity, i});
      this->rp3d_body_index_to_ecs_entity_map.insert({i, entity});

      break;
    }
  }

  afk_assert(
      this->rp3d_body_id_to_ecs_entity_map.count(body->getEntity().id) == 0,
      "ReactPhysics body id has already being mapped to an AFK ECS Entity");
  this->rp3d_body_id_to_ecs_entity_map.insert({body->getEntity().id, entity});

  // add all colliders to rp3d collision body
  for (const auto &collision_body : collider_component.colliders) {
    // combine collider transform scale with parent transform
    // need to apply parent scale at the shape level, as scale cannot be applied to the parent body level
    auto collision_transform = collision_body.transform;
    collision_transform.scale.x *= transform_component.scale.x;
    collision_transform.scale.y *= transform_component.scale.y;
    collision_transform.scale.z *= transform_component.scale.z;

    // create transform for collider (this does NOT include scale as reactphysics does not have scale in its transform, to get around this the scale is manually added to the collision shapes)
    const auto rp3d_transform =
        rp3d::Transform(rp3d::Vector3(collision_transform.translation.x,
                                      collision_transform.translation.y,
                                      collision_transform.translation.z),
                        rp3d::Quaternion(collision_transform.rotation.x,
                                         collision_transform.rotation.y,
                                         collision_transform.rotation.z,
                                         collision_transform.rotation.w));

    // add collider based on collider type
    auto visitor = afk::utility::Visitor{
        [this, &collision_transform, &rp3d_transform, &body](afk::physics::shape::Box shape) {
          // add rp3d shape and rp3d transform to collider
          body->addCollider(this->create_shape_box(shape, collision_transform.scale),
                            rp3d_transform);
        },
        [this, &collision_transform, &rp3d_transform, &body](afk::physics::shape::Sphere shape) {
          // add rp3d shape and rp3d transform to collider
          body->addCollider(this->create_shape_sphere(shape, collision_transform.scale),
                            rp3d_transform);
        },
        [this, &collision_transform, &rp3d_transform, &body](afk::physics::shape::Capsule shape) {
          // add rp3d shape and rp3d transform to collider
          body->addCollider(
              this->create_shape_capsule(shape, collision_transform.scale), rp3d_transform);
        },
        [](auto) { afk_unreachable(); }};

    std::visit(visitor, collision_body.shape);
  }
}

auto CollisionSystem::get_debug_mesh() -> afk::render::Mesh {
  afk::render::Mesh mesh     = {};
  mesh.transform.translation = glm::vec3{0.0f};

  auto debug_renderer = &this->world->getDebugRenderer();
  auto triangles      = debug_renderer->getTriangles();

  // note: some points may be duplicated
  auto no_vertices = usize{0};
  for (auto i = usize{0}; i < triangles.size(); ++i) {
    auto vertex1     = afk::render::Vertex{};
    vertex1.position = glm::vec3{triangles[i].point1.x, triangles[i].point1.y,
                                 triangles[i].point1.z};
    mesh.vertices.push_back(vertex1);
    auto vertex2     = afk::render::Vertex{};
    vertex2.position = glm::vec3{triangles[i].point2.x, triangles[i].point2.y,
                                 triangles[i].point2.z};
    mesh.vertices.push_back(vertex2);
    auto vertex3     = afk::render::Vertex{};
    vertex3.position = glm::vec3{triangles[i].point3.x, triangles[i].point3.y,
                                 triangles[i].point3.z};
    mesh.vertices.push_back(vertex3);

    mesh.indices.emplace_back(no_vertices);
    ++no_vertices;
    mesh.indices.emplace_back(no_vertices);
    ++no_vertices;
    mesh.indices.emplace_back(no_vertices);
    ++no_vertices;
  }

  return mesh;
}

rp3d::BoxShape *CollisionSystem::create_shape_box(const afk::physics::shape::Box &box,
                                                  const glm::vec3 &scale) {
  return this->physics_common.createBoxShape(
      rp3d::Vector3(box.x * scale.x, box.y * scale.y, box.z * scale.z));
}

rp3d::SphereShape *CollisionSystem::create_shape_sphere(const afk::physics::shape::Sphere &sphere,
                                                        const glm::vec3 &scale) {
  const auto scale_factor = (scale.x + scale.y + scale.z) / 3.0f;
  return this->physics_common.createSphereShape(sphere * scale_factor);
}

rp3d::CapsuleShape *CollisionSystem::create_shape_capsule(const afk::physics::shape::Capsule &capsule,
                                                          const glm::vec3 &scale) {
  return this->physics_common.createCapsuleShape(
      capsule.radius * ((scale.x + scale.y) / 2.0f), capsule.height * scale.y);
}

void CollisionSystem::CollisionEventListener::onContact(
    const rp3d::CollisionCallback::CallbackData &callback_data) {
  // On collision event, there will be two colliders colliding
  // Iterate over all these pairs
  for (rp3d::uint p = 0; p < callback_data.getNbContactPairs(); p++) {

    // Get the contact pair
    const auto contact_pair = callback_data.getContactPair(p);

    auto &engine         = afk::Engine::get();
    const auto &registry = engine.ecs.registry;

    // get the AFK ECS entities of the colliders
    const auto body_to_ecs_map = &engine.collision_system.rp3d_body_id_to_ecs_entity_map;
    const auto body1IdMapIterator =
        body_to_ecs_map->find(contact_pair.getBody1()->getEntity().id);
    afk_assert(body1IdMapIterator != body_to_ecs_map->end(),
               "Could not find body 1 id in rp3d_body_to_ecs_map");
    const auto object1 = body1IdMapIterator->second;

    auto body2IdMapIterator =
        body_to_ecs_map->find(contact_pair.getBody2()->getEntity().id);
    afk_assert(body2IdMapIterator != body_to_ecs_map->end(),
               "Could not find body 2 id in rp3d_body_to_ecs_map");
    auto object2 = body2IdMapIterator->second;

    // check that the colliders do not belong to the same entity in react physics 3d
    if (object1 != object2) {
      // entities without physics components are considered to be static
      // if both objects are static, don't bother to fire an event
      if (registry.has<PhysicsComponent>(object1) && registry.has<PhysicsComponent>(object2)) {

        auto &event_manager = engine.event_manager;

        // fire collision events
        // note that if a collision body is "sleeping" in reactphysics3d, a
        // collision event of type ContactStay will not fire at the moment,
        // "sleeping" is disabled treat contact enter and contact stay as "impulses"

        if (contact_pair.getEventType() == CollisionCallback::ContactPair::EventType::ContactStart ||
            contact_pair.getEventType() ==
                CollisionCallback::ContactPair::EventType::ContactStay) {
          auto data = afk::event::Event::Collision{object1, object2, {}};

          afk_assert(contact_pair.getNbContactPoints() > 0,
                     "No contact points found on collision");

          data.contacts.reserve(contact_pair.getNbContactPoints());
          for (u32 i = 0; i < contact_pair.getNbContactPoints(); ++i) {
            const auto &contact_point = contact_pair.getContactPoint(i);
            const auto collider1_local_point =
                glm::vec3{contact_point.getLocalPointOnCollider1().x,
                          contact_point.getLocalPointOnCollider1().y,
                          contact_point.getLocalPointOnCollider1().z};
            const auto collider2_local_point =
                glm::vec3{contact_point.getLocalPointOnCollider2().x,
                          contact_point.getLocalPointOnCollider2().y,
                          contact_point.getLocalPointOnCollider2().z};
            const auto contact_normal = glm::vec3{contact_point.getWorldNormal().x,
                                                  contact_point.getWorldNormal().y,
                                                  contact_point.getWorldNormal().z};

            data.contacts.push_back(afk::event::Event::Collision::Contact{
                collider1_local_point, collider2_local_point, contact_normal,
                contact_point.getPenetrationDepth()});
          }

          event_manager.push_event(afk::event::Event{data, afk::event::Event::Type::Collision});
        }
      }
    }
  }
}

void CollisionSystem::Logger::log(rp3d::Logger::Level level, const std::string &physicsWorldName,
                                  rp3d::Logger::Category category, const std::string &message,
                                  const char *filename, int lineNumber) {
  afk::io::log << "[" << getLevelName(level) << "] " << message << "\n";
}
