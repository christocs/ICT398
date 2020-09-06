#include "afk/physics/PhysicsBodySystem.hpp"

#include <iostream>
#include <unordered_map>

#include "afk/Afk.hpp"
#include "afk/NumericTypes.hpp"
#include "afk/debug/Assert.hpp"
#include "afk/event/Event.hpp"
#include "afk/io/Log.hpp"

using afk::physics::PhysicsBodySystem;
using afk::physics::Transform;

using afk::render::debug::PhysicsView;

PhysicsBodySystem::PhysicsBodySystem() {
  this->world = this->physics_common.createPhysicsWorld();
  this->world->setIsGravityEnabled(false);
//  this->world->setEventListener(&listener);
  this->world->setIsDebugRenderingEnabled(true);
  // todo: turn it back on
  this->world->enableSleeping(false);

  // Create the default logger
  //  rp3d::DefaultLogger *logger = this->physics_common.createDefaultLogger();

  // Log level (warnings and errors)
  auto logLevel =
      static_cast<afk::u16>(static_cast<afk::u16>(rp3d::Logger::Level::Information) |
                            static_cast<afk::u16>(rp3d::Logger::Level::Warning) |
                            static_cast<afk::u16>(rp3d::Logger::Level::Error));

  // Output the logs into an HTML file
  //    logger->addStreamDestination(afk::io::log, logLevel, rp3d::DefaultLogger::Format::Text);

  // Output the logs into an HTML file
  //  logger->addFileDestination("rp3d_log_.html", logLevel, rp3d::DefaultLogger::Format::HTML);

  // Set the logger
  this->physics_common.setLogger(&(this->logger));

  //  this->set_debug_physics_item(afk::render::debug::PhysicsView::CONTACT_NORMAL, true);
  //  this->set_debug_physics_item(afk::render::debug::PhysicsView::CONTACT_POINT, true);
  //  this->set_debug_physics_item(afk::render::debug::PhysicsView::COLLISION_SHAPE, true);
  //  this->set_debug_physics_item(afk::render::debug::PhysicsView::COLLIDER_BROADPHASE_AABB, true);
  //  this->set_debug_physics_item(afk::render::debug::PhysicsView::COLLIDER_AABB, true);

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
  this->physics_common.destroyPhysicsWorld(this->world);
}

auto PhysicsBodySystem::update(float dt) -> void {
  // don't call update(dt) unless needing to show debug renderer
  this->world->update(dt);
  this->world->testCollision(this->collision_callback);

  auto registry = &afk::Engine::get().registry;
  auto physics_body_view =
      registry->view<afk::physics::PhysicsBody, afk::physics::Transform>();

  // apply gravity acceleration + update pos
  for (auto &entity : physics_body_view) {
    auto body = physics_body_view.get<afk::physics::PhysicsBody>(entity);
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

  // todo: instead of creating new models to store, replace the existing debug
  // model in OpenGL
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

  // todo: update reactphysics world with where the object changes position in the game world
}

void PhysicsBodySystem::CollisionCallback::onContact(
    const rp3d::CollisionCallback::CallbackData &callback_data) {
  // For each contact pair
  for (rp3d::uint p = 0; p < callback_data.getNbContactPairs(); p++) {

    // Get the contact pair
    const auto contact_pair = callback_data.getContactPair(p);

    auto engine   = &afk::Engine::get();
    auto registry = &engine->registry;

    const auto body_to_ecs_map = &engine->physics_body_system.rp3d_body_to_ecs_map;
    auto body1IdMapIterator =
        body_to_ecs_map->find(contact_pair.getBody1()->getEntity().id);
    afk_assert(body1IdMapIterator != body_to_ecs_map->end(),
               "Could not find body 1 id in rp3d_body_to_ecs_map");
    auto object1 = body1IdMapIterator->second;

    auto body2IdMapIterator =
        body_to_ecs_map->find(contact_pair.getBody2()->getEntity().id);
    afk_assert(body2IdMapIterator != body_to_ecs_map->end(),
               "Could not find body 2 id in rp3d_body_to_ecs_map");
    auto object2 = body2IdMapIterator->second;

    // only proceed if not colliding with self
    if (object1 != object2) {
      auto body1         = registry->get<afk::physics::PhysicsBody>(object1);
      auto body2         = registry->get<afk::physics::PhysicsBody>(object2);
      auto event_manager = &engine->event_manager;

      // only bother processing contacts between two non-static objects
      if (body1.type != BodyType::Static || body2.type != BodyType::Static) {

        // treat contact enter and contact stay as "impulses"
        // note: this implementation requires rigid bodies to never sleep
        if (contact_pair.getEventType() == CollisionCallback::ContactPair::EventType::ContactStart ||
            contact_pair.getEventType() ==
            CollisionCallback::ContactPair::EventType::ContactStay) {
          auto data = afk::event::Event::CollisionImpulse{
              afk::event::Event::CollisionImpulseBodyData{body1.type, object1,
                                                          glm::vec3{1.0}},
              afk::event::Event::CollisionImpulseBodyData{body2.type, object2,
                                                          glm::vec3{1.0}},
              {}};

          afk_assert(contact_pair.getNbContactPoints() > 0,
                     "No contact points found on collision");

          data.collision_normals.reserve(contact_pair.getNbContactPoints());
          for (u32 i = 0; i < contact_pair.getNbContactPoints(); ++i) {
            auto contact_normal = contact_pair.getContactPoint(i).getWorldNormal();
            data.collision_normals.emplace_back(contact_normal.x, contact_normal.y,
                                                contact_normal.z);
          }
          afk::io::log << "fire collision ************ \n";

          event_manager->push_event(
              afk::event::Event{data, afk::event::Event::Type::CollisionImpulse});
        }
      }
    }
  }
}

auto PhysicsBodySystem::set_debug_physics_item(const PhysicsView &physics_view,
                                               bool status) -> void {
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
  afk::render::Mesh mesh     = {};
  mesh.transform.translation = glm::vec3{0.0f};

  auto debug_renderer = &this->world->getDebugRenderer();
  auto triangles      = debug_renderer->getTriangles();
  //  afk::io::log << "no triangles: " << std::to_string(triangles.size()) << "\n";
  //  afk::io::log << "no lines: " << std::to_string(debug_renderer->getNbLines()) << "\n";

  // note: some points may be duplicated
  auto no_vertices = usize{0};
  for (auto i = usize{0}; i < triangles.size(); ++i) {
    //    afk::io::log << "no i: " << std::to_string(i) << "\n";
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
  afk::io::log << "[" << getLevelName(level) << "] " << message << "\n";
}
