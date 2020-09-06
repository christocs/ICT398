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
  this->world->setEventListener(&listener);
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

auto debug_count = usize{0};
auto PhysicsBodySystem::update(float dt) -> void {
  this->world->update(dt);

  auto debug_mesh = this->get_debug_mesh();
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

void PhysicsBodySystem::CollisionEventListener::onContact(
    const rp3d::CollisionCallback::CallbackData &callback_data) {
  // For each contact pair
  for (rp3d::uint p = 0; p < callback_data.getNbContactPairs(); p++) {

    // Get the contact pair
    CollisionCallback::ContactPair contactPair = callback_data.getContactPair(p);

    auto engine       = &afk::Engine::get();
    auto registry     = &engine->registry;

    const auto body_to_ecs_map = &engine->physics_body_system.rp3d_body_to_ecs_map;
    auto body1IdMapIterator = body_to_ecs_map->find(contactPair.getBody1()->getEntity().id);
    afk_assert(body1IdMapIterator != body_to_ecs_map->end(), "Could not find body 1 id in rp3d_body_to_ecs_map");
    auto object1 = body1IdMapIterator->second;
    
    auto body2IdMapIterator = body_to_ecs_map->find(contactPair.getBody2()->getEntity().id);
    afk_assert(body2IdMapIterator != body_to_ecs_map->end(), "Could not find body 2 id in rp3d_body_to_ecs_map");
    auto object2 = body2IdMapIterator->second;

    // only proceed if not colliding with self
    if (object1 != object2) {
      auto body1         = registry->get<afk::physics::PhysicsBody>(object1);
      auto body2         = registry->get<afk::physics::PhysicsBody>(object2);
      auto event_manager = &engine->event_manager;

      if (contactPair.getEventType() == CollisionCallback::ContactPair::EventType::ContactStart) {
        auto data = afk::event::Event::CollisionImpulse{};
        data[0].type = body1.type;
        data[0].body_id = object1;
        data[1].type = body2.type;
        data[1].body_id = object2;
        event_manager->push_event(afk::event::Event{
          data,
          afk::event::Event::Type::CollisionImpulse
        });
        //        std::vector<glm::vec3> contact_points = {};
        //        contact_points.reserve(contactPair.getNbContactPoints());
        //        for (u32 i = 0; i < contactPair.getNbContactPoints(); ++i) {
        //          auto contact_point = contactPair.getContactPoint(i).getWorldNormal();
        //          contact_points.emplace_back(contact_point.x, contact_point.y,
        //                                      contact_point.z);
        //        }
        //        afk::Engine::get().physics_body_system.collision_enter_queue.emplace(
        //            PhysicsBodySystem::Collision{object1, object2, std::move(contact_points)});
      } else if (contactPair.getEventType() ==
                 CollisionCallback::ContactPair::EventType::ContactExit) {
        //        std::vector<glm::vec3> contact_points = {};
        //        contact_points.reserve(contactPair.getNbContactPoints());
        //        for (u32 i = 0; i < contactPair.getNbContactPoints(); ++i) {
        //          auto contact_point = contactPair.getContactPoint(i).getWorldNormal();
        //          contact_points.emplace_back(contact_point.x, contact_point.y,
        //                                      contact_point.z);
        //        }
        //        afk::Engine::get().physics_body_system.collision_enter_queue.emplace(
        //            PhysicsBodySystem::Collision{object1, object2, std::move(contact_points)});
      } else {
        // ContactStay may not be triggered if rigid body is sleeping
        auto data = afk::event::Event::CollisionImpulse{};
        data[0].type = body1.type;
        data[0].body_id = object1;
        data[1].type = body2.type;
        data[1].body_id = object2;
        event_manager->push_event(afk::event::Event{
            data,
            afk::event::Event::Type::CollisionImpulse
        });
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
