#include "afk/physics/PhysicsBodySystem.hpp"

#include <unordered_map>

#include "afk/Afk.hpp"
#include "afk/debug/Assert.hpp"

using afk::physics::PhysicsBodySystem;
using afk::physics::Transform;

using afk::render::debug::PhysicsView;

PhysicsBodySystem::PhysicsBodySystem() {
  this->world = this->physics_common.createPhysicsWorld();
  this->world->setIsGravityEnabled(false);
  //  this->world->setEventListener(&listener); // an event listener can be used to fire events instead of manually firing a test for collisions, more detail in the update function
  this->world->setIsDebugRenderingEnabled(true);
  // rp3d puts collision bodies to "sleep" after they have been contacting for a while, for now disable this optimisation feature to make dev work simpler
  this->world->enableSleeping(false);

  // Set the cli logger
  this->physics_common.setLogger(&(this->logger));

  // set debug physics items properly through the engine instead of doing it raw
  //  this->set_debug_physics_item(afk::render::debug::PhysicsView::CONTACT_NORMAL, true);
  //  this->set_debug_physics_item(afk::render::debug::PhysicsView::CONTACT_POINT, true);
  //  this->set_debug_physics_item(afk::render::debug::PhysicsView::COLLISION_SHAPE, true);
  //  this->set_debug_physics_item(afk::render::debug::PhysicsView::COLLIDER_BROADPHASE_AABB, true);
  //  this->set_debug_physics_item(afk::render::debug::PhysicsView::COLLIDER_AABB, true);

  // setting debug physics items raw
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
}

PhysicsBodySystem::~PhysicsBodySystem() {
  // unnecessarily destroy the physics world manually, this will be done automatically by physics common when physics common is destroyed (which will be the same time as a default destructor)
  this->physics_common.destroyPhysicsWorld(this->world);
}

auto PhysicsBodySystem::update(float dt) -> void {
  // update is required for updating the debug renderer data within rp3d
  // it also fires event collision data, which would be nice to use instead of manually calling testCollision so collisions don't get tested twice
  this->world->update(dt);

  // manually fire a test to check collisions, utilising the collision callback we defined earlier to fire collisions as system events
  // test collisions AFTER the existing velocity has been applied, so it can fire events to update for the next frame
  // todo: check if this puts physics 1 frame out of sync (maybe solution should be handled first?)
  this->world->testCollision(this->collision_callback);

  auto registry = &afk::Engine::get().registry;
  auto physics_body_view =
      registry->view<afk::physics::PhysicsBody, afk::physics::Transform>();

  // apply gravity acceleration
  // apply velocity of physicsbody to the transform component
  // apply new position according to velocity to the rp3d collision body
  for (auto &entity : physics_body_view) {
    auto body = physics_body_view.get<afk::physics::PhysicsBody>(entity);
    // only bother doing this to non-static objects, as static objects will not move (probably should still set the positino of static objects just inc ase they change anyway though)
    if (body.type != afk::physics::BodyType::Static) {
      body.velocity.x += (dt * this->gravity.x);
      body.velocity.y += (dt * this->gravity.y);
      body.velocity.z += (dt * this->gravity.z);

      // doing the below in a separate loop doesn't work, possibly a race condition of getting the velocity data?
      auto transform = physics_body_view.get<afk::physics::Transform>(entity);
      transform.translation.x += (dt * body.velocity.x);
      transform.translation.y += (dt * body.velocity.y);
      transform.translation.z += (dt * body.velocity.z);

      body.set_pos(transform.translation);
    }
  }

  // create debug model based on rp3d debug mesh
  // todo: instead of creating new models to store, replace the existing debug
  // todo: also get debug lines from reactphysics3d
  static auto debug_count = usize{0};
  auto debug_mesh         = this->get_debug_mesh();
  if (this->model.meshes.empty()) {
    this->model.meshes.push_back(std::move(debug_mesh));
  } else {
    this->model.meshes[0] = std::move(debug_mesh);
  }
  this->model.file_dir  = "debug";
  this->model.file_path = "debug/" + std::to_string(debug_count);
  ++debug_count;
}

void PhysicsBodySystem::CollisionCallback::onContact(const rp3d::CollisionCallback::CallbackData &callback_data) {
  // On collision event, there will be two colliders colliding
  // Iterate over all these pairs
  for (rp3d::uint p = 0; p < callback_data.getNbContactPairs(); p++) {

    // Get the contact pair
    const auto contact_pair = callback_data.getContactPair(p);

    auto engine   = &afk::Engine::get();
    auto registry = &engine->registry;

    // get the AFK ECS entities of the colliders
    const auto body_to_ecs_map = &engine->physics_body_system.rp3d_body_to_ecs_map;
    afk_assert(body_to_ecs_map->count(contact_pair.getBody1()->getEntity().id) > 0,
               "Could not find body 1 id in rp3d_body_to_ecs_map");
    auto body1IdMapIterator =
        body_to_ecs_map->find(contact_pair.getBody1()->getEntity().id);
    auto object1 = body1IdMapIterator->second;

    afk_assert(body_to_ecs_map->count(contact_pair.getBody2()->getEntity().id) > 0,
               "Could not find body 2 id in rp3d_body_to_ecs_map");
    auto body2IdMapIterator =
        body_to_ecs_map->find(contact_pair.getBody2()->getEntity().id);
    auto object2 = body2IdMapIterator->second;

    // check that the colliders do not belong to the same collision body in react physics 3d
    // note that there is a difference between a collision body and a collider
    // a collider is a single shape, while a collision body may be made of multiple colliders
    // the physics body component represents a rp3d collision body
    if (object1 != object2) {
      // get the physics body components
      auto body1         = registry->get<afk::physics::PhysicsBody>(object1);
      auto body2         = registry->get<afk::physics::PhysicsBody>(object2);
      auto event_manager = &engine->event_manager;

      // fire collision events for collision impulses
      // treat contact enter and contact stay as "impulses"
      // note that if a collision body is "sleeping" in reactphysics3d, a collision event of type ContactStay will not fire
      // at the moment, "sleeping" is disabled
      if (contact_pair.getEventType() == CollisionCallback::ContactPair::EventType::ContactStart ||
          contact_pair.getEventType() == CollisionCallback::ContactPair::EventType::ContactStay) {
        auto data = afk::event::Event::CollisionImpulse{
            afk::event::Event::CollisionImpulse::CollisionImpulseBodyData{
                object1, glm::vec3{1.0}},
            afk::event::Event::CollisionImpulse::CollisionImpulseBodyData{
                object2, glm::vec3{1.0}},
            {}};

        afk_assert(contact_pair.getNbContactPoints() > 0,
                   "No contact points found on collision");

        // get the collision normals and put it into the event data
        data.collision_normals.reserve(contact_pair.getNbContactPoints());
        for (u32 i = 0; i < contact_pair.getNbContactPoints(); ++i) {
          auto contact_normal = contact_pair.getContactPoint(i).getWorldNormal();
          data.collision_normals.emplace_back(contact_normal.x, contact_normal.y,
                                              contact_normal.z);
        }
        // display collision to the log
        afk::io::log << "fire collision ************ \n";

        // push the event
        event_manager->push_event(
            afk::event::Event{data, afk::event::Event::Type::CollisionImpulse});
      }
    }
  }
}

auto PhysicsBodySystem::resolve_collision_event(const afk::event::Event::CollisionImpulse &data)
    -> void {
  //  make dynamic bodies come to a halt
  //  auto registry = &afk::Engine::get().registry;
  //  auto body1 = registry->get<afk::physics::PhysicsBody>(data.body1.id);
  //  auto body2 = registry->get<afk::physics::PhysicsBody>(data.body2.id);
  //  if (body1.type == afk::physics::BodyType::Dynamic) {
  //    body1.velocity = glm::vec3{0.0f, 0.0f, 0.0f};
  //  }
  //  if (body2.type == afk::physics::BodyType::Dynamic) {
  //    body2.velocity = glm::vec3{0.0f, 0.0f, 0.0f};
  //  }
}

auto PhysicsBodySystem::set_debug_physics_item(const PhysicsView &physics_view,
                                               bool status) -> void {
  // set rp3d debug items to bother generating data for

  rp3d::DebugRenderer::DebugItem rp3d_debug_item = {};

  switch (physics_view) {
    case PhysicsView::COLLIDER_AABB:
      rp3d_debug_item = rp3d::DebugRenderer::DebugItem::COLLIDER_AABB;
      break;
    case PhysicsView::COLLIDER_BROADPHASE_AABB:
      rp3d_debug_item = rp3d::DebugRenderer::DebugItem::COLLIDER_BROADPHASE_AABB;
      break;
    case PhysicsView::COLLISION_SHAPE:
      rp3d_debug_item = rp3d::DebugRenderer::DebugItem::COLLISION_SHAPE;
      break;
    case PhysicsView::CONTACT_POINT:
      rp3d_debug_item = rp3d::DebugRenderer::DebugItem::CONTACT_POINT;
      break;
    case PhysicsView::CONTACT_NORMAL:
      rp3d_debug_item = rp3d::DebugRenderer::DebugItem::CONTACT_NORMAL;
      break;
  }

  afk::Engine::get().physics_body_system.world->getDebugRenderer().setIsDebugItemDisplayed(
      rp3d_debug_item, status);
}

auto PhysicsBodySystem::get_debug_physics_item(const afk::render::debug::PhysicsView &physics_view) const
    -> bool {
  rp3d::DebugRenderer::DebugItem rp3d_debug_item;
  switch (physics_view) {
    case PhysicsView::COLLIDER_AABB:
      rp3d_debug_item = rp3d::DebugRenderer::DebugItem::COLLIDER_AABB;
      break;
    case PhysicsView::COLLIDER_BROADPHASE_AABB:
      rp3d_debug_item = rp3d::DebugRenderer::DebugItem::COLLIDER_BROADPHASE_AABB;
      break;
    case PhysicsView::COLLISION_SHAPE:
      rp3d_debug_item = rp3d::DebugRenderer::DebugItem::COLLISION_SHAPE;
      break;
    case PhysicsView::CONTACT_POINT:
      rp3d_debug_item = rp3d::DebugRenderer::DebugItem::CONTACT_POINT;
      break;
    case PhysicsView::CONTACT_NORMAL:
      rp3d_debug_item = rp3d::DebugRenderer::DebugItem::CONTACT_NORMAL;
      break;
  }

  this->world->getDebugRenderer().getIsDebugItemDisplayed(rp3d_debug_item);
}

auto PhysicsBodySystem::get_debug_model() -> afk::render::Model {
  return this->model;
}

auto PhysicsBodySystem::get_debug_mesh() -> afk::render::Mesh {
  // take rp3d's silly, dumb mesh, and put it quickly into an AFK mesh
  // rp3d makes no attempt to share positions, so neither does this, don't want to waste a silly amount of time finding shared points
  afk::render::Mesh mesh     = {};
  mesh.transform.translation = glm::vec3{0.0f};

  auto debug_renderer = &this->world->getDebugRenderer();
  auto triangles      = debug_renderer->getTriangles();
  //  afk::io::log << "no triangles: " << std::to_string(triangles.size()) << "\n";
  //  afk::io::log << "no lines: " << std::to_string(debug_renderer->getNbLines()) << "\n";

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

void PhysicsBodySystem::Logger::log(Level level, const std::string &physicsWorldName,
                                    Category category, const std::string &message,
                                    const char *filename, int lineNumber) {
  // log events
  afk::io::log << "[" << getLevelName(level) << "] " << message << "\n";
}
