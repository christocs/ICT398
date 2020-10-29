#include "afk/ecs/system/CollisionSystem.hpp"

#include "afk/Engine.hpp"
#include "afk/debug/Assert.hpp"
#include "afk/ecs/component/PhysicsComponent.hpp"
#include "afk/io/Log.hpp"
#include "afk/io/Time.hpp"
#include "afk/render/WireframeMesh.hpp"
#include "afk/utility/Visitor.hpp"

using glm::vec3;
using glm::vec4;

using afk::ecs::component::ColliderComponent;
using afk::ecs::component::PhysicsComponent;
using afk::ecs::component::TransformComponent;
using afk::ecs::system::CollisionSystem;
using afk::render::Index;
using afk::render::WireframeMesh;

CollisionSystem::CollisionSystem() {

  // Set the debug logger for events in ReactPhysics3D
  this->physics_common.setLogger(&(this->logger));
}

auto CollisionSystem::initialize() -> void {
  afk_assert(!this->is_initialized, "Collision System already initialized");
  afk::io::log << afk::io::get_date_time() << "Collision System initialized\n";

  // setup callback for when a ColliderComponent is destroyed so it can cleanup data related to the component
  auto &registry = afk::Engine::get().ecs.registry;
  registry.on_destroy<ColliderComponent>().connect<&CollisionSystem::on_collider_destroy>();

  // instantiate the reactphysics3d world
  this->world = this->create_rp3d_physics_world();

  this->is_initialized = true;
}

auto CollisionSystem::on_collider_destroy(afk::ecs::Registry &registry,
                                          afk::ecs::Entity entity) -> void {
  auto &engine           = afk::Engine::get();
  auto &collision_system = engine.collision_system;

  afk_assert(collision_system.ecs_entity_to_rp3d_body_index_map.count(entity) == 1,
             "rp3d body index not mapped to ecs entity");
  const auto collider_index =
      collision_system.ecs_entity_to_rp3d_body_index_map.at(entity);
  auto collider = collision_system.world->getCollisionBody(collider_index);

  // grab the reactphysics3d internal entity for later when we want to delete the mappings
  auto rp3d_id_entity = collider->getEntity().id;

  // destroy the body in reactphysics3d
  collision_system.world->destroyCollisionBody(collider);

  // destroy the mappings of reactphysics3d objects
  collision_system.ecs_entity_to_rp3d_body_index_map.erase(
      entity); // already guarenteed to be present earlier in the method, so no need to check if it exists again

  // while the program *may* run fine if these mappings are not deleted, it may run into undefined behaviour and possibly fail elsewhere, hence the use of assertions
  // if the mappings are not deleted, only chuck a fuss in debug mode, but still log it as an error as it is concerning
  if (collision_system.rp3d_body_index_to_ecs_entity_map.count(collider_index) == 1) {
    collision_system.rp3d_body_index_to_ecs_entity_map.erase(collider_index);
  } else {
    afk::io::log << "Warning: Failed to find rp3d body index in "
                    "rp3d_boxy_index_to_ecs_entity_map\n";
    afk_assert_debug(false, "Warning: Failed to find rp3d body index in "
                            "rp3d_boxy_index_to_ecs_entity_map\n");
  }
  if (collision_system.rp3d_body_id_to_ecs_entity_map.count(rp3d_id_entity) == 1) {
    collision_system.rp3d_body_id_to_ecs_entity_map.erase(rp3d_id_entity);
  } else {
    afk::io::log << "Warning: Failed to find rp3d id in "
                    "rp3d_body_id_to_ecs_entity_map\n";
    afk_assert_debug(false, "Warning: Failed to find rp3d id in "
                            "rp3d_body_id_to_ecs_entity_map\n");
  }

  // if the entity also has a PhysicsComponent, also delete that component as the PhysicsComponent should always have a ColliderComponent
  registry.remove_if_exists<PhysicsComponent>(entity);
}

auto CollisionSystem::update() -> void {
  auto &afk = afk::Engine::get();

  this->syncronize_colliders();

  this->update_camera_raycast();

  // update React3DPhysics world
  // this method calls to update the debug render data
  // this method fires collision events
  // this method also unnecessarily does physics calculations for any rigid bodies, though none should be created in the game engine
  this->world->update(afk.get_delta_time());
}

auto CollisionSystem::syncronize_colliders() -> void {
  auto &afk      = afk::Engine::get();
  auto &registry = afk.ecs.registry;

  // update every collider component, regardless if it has a physics component or not (transform for an entity may need to be moved regardless)
  auto collider_view = registry.view<ColliderComponent, TransformComponent>();

  // update translation and rotation in react physics 3d representation
  // @todo apply scale dynamically, most likely need to trigger a change and at that point make new rp3d shapes that are scaled
  for (auto &entity : collider_view) {
    const auto &collider = collider_view.get<ColliderComponent>(entity);
    auto &transform      = collider_view.get<TransformComponent>(entity);
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

    // normalize rotation
    transform.rotation = glm::normalize(transform.rotation);
  }
}

auto CollisionSystem::update_camera_raycast() -> void {
  auto &afk = afk::Engine::get();

  // set camera raycast entity to nothing
  afk.camera.set_raycast_entity(std::nullopt);

  // throw away unnecessary raycast info left ovver
  this->camera_raycast_info.clear();

  // build camera raycast
  const auto camera_front = afk.camera.get_front();
  const auto camera_front_rp3d =
      rp3d::Vector3(camera_front.x, camera_front.y, camera_front.z);
  const auto camera_pos = afk.camera.get_position();
  const auto camera_pos_rp3d =
      rp3d::Vector3(camera_pos.x, camera_pos.y, camera_pos.z);

  auto camera_ray =
      rp3d::Ray(camera_front_rp3d * afk.camera.get_near() + camera_pos_rp3d,
                camera_front_rp3d * afk.camera.get_far() + camera_pos_rp3d);

  // populate raycast info
  this->world->raycast(camera_ray, &raycast_callback);

  // process raycast hit data if any exists, get the closest one to set in the camera, then delete the data
  if (!this->camera_raycast_info.empty()) {
    auto shortest_raycast_index = size_t{0};
    for (auto i = size_t{0}; i < this->camera_raycast_info.size(); ++i) {
      // if hit fraction is lower, that means it is closer to the camera, hence set that as the shortest raycast
      if (this->camera_raycast_info[i].hit_fraction <
          this->camera_raycast_info[shortest_raycast_index].hit_fraction) {
        shortest_raycast_index = i;
      }
    }

    const auto &shortest_raycast = this->camera_raycast_info[shortest_raycast_index];
    afk_assert(shortest_raycast.collision_body != nullptr,
               "Raycast body is null");
    const auto hit_body_id = shortest_raycast.collision_body->getEntity().id;

    afk_assert(this->rp3d_body_id_to_ecs_entity_map.count(hit_body_id) == 1,
               "Raycast hit a collider that is not mapped");

    const auto hit_entity = this->rp3d_body_id_to_ecs_entity_map.at(hit_body_id);

    afk.camera.set_raycast_entity(hit_entity);

    // throw away unnecessary raycast info
    this->camera_raycast_info.clear();
  }
}

auto CollisionSystem::instantiate_collider_component(
    const afk::ecs::Entity &entity, afk::ecs::component::ColliderComponent &collider_component,
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
  for (auto i = u32{0}; i < no_collision_bodies; ++i) {
    if (this->world->getCollisionBody(i)->getEntity().id == body->getEntity().id) {
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

  for (const auto &collision_body : collider_component.colliders) {
    // combine collider transform scale with parent transform
    // need to apply parent scale at the shape level, as scale cannot be applied to the parent body level
    auto collision_transform = collision_body.transform;
    collision_transform.scale *= transform_component.scale;

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
        [](auto) { afk_unreachable(); }};

    std::visit(visitor, collision_body.shape);
  }
}

static auto u32_color_to_vec4(u32 color) -> vec4 {
  constexpr auto red_bits        = u32{0xFF000000};
  constexpr auto green_bits      = u32{0x00FF0000};
  constexpr auto blue_bits       = u32{0x0000FF00};
  constexpr auto alpha_bits      = u32{0x000000FF};
  constexpr auto max_color_value = static_cast<f32>(0xFF);

  const auto red   = static_cast<f32>((color & red_bits) >> 24);
  const auto green = static_cast<f32>((color & green_bits) >> 16);
  const auto blue  = static_cast<f32>((color & blue_bits) >> 8);
  const auto alpha = static_cast<f32>(color & alpha_bits);

  return vec4{red / max_color_value, green / max_color_value,
              blue / max_color_value, alpha / max_color_value};
}

auto CollisionSystem::get_regular_debug_mesh() -> afk::render::Mesh {
  afk::render::Mesh mesh     = {};
  mesh.transform.translation = glm::vec3{0.0f};

  auto debug_renderer = &this->world->getDebugRenderer();
  auto triangles      = debug_renderer->getTriangles();

  // note: some points may be duplicated
  auto no_vertices = u32{0};
  for (auto i = u32{0}; i < triangles.size(); ++i) {
    auto vertex1     = afk::render::Mesh::Vertex{};
    vertex1.position = glm::vec3{triangles[i].point1.x, triangles[i].point1.y,
                                 triangles[i].point1.z};
    mesh.vertices.push_back(vertex1);
    auto vertex2     = afk::render::Mesh::Vertex{};
    vertex2.position = glm::vec3{triangles[i].point2.x, triangles[i].point2.y,
                                 triangles[i].point2.z};
    mesh.vertices.push_back(vertex2);
    auto vertex3     = afk::render::Mesh::Vertex{};
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

auto CollisionSystem::get_debug_mesh() -> WireframeMesh {
  WireframeMesh mesh         = {};
  mesh.transform.translation = vec3{0.0f};

  const auto vertices_per_triangle = 3;
  const auto debug_renderer        = &this->world->getDebugRenderer();
  const auto triangles             = debug_renderer->getTriangles();
  auto num_vertices                = usize{0};

  // Note: some points may be duplicated

  for (auto i = usize{0}; i < triangles.size(); ++i) {
    const auto &t = triangles[static_cast<u32>(i)];

    const auto v1 =
        WireframeMesh::Vertex{.position = {t.point1.x, t.point1.y, t.point1.z},
                              .color    = u32_color_to_vec4(t.color1)};
    const auto v2 =
        WireframeMesh::Vertex{.position = {t.point2.x, t.point2.y, t.point2.z},
                              .color    = u32_color_to_vec4(t.color2)};

    const auto v3 =
        WireframeMesh::Vertex{.position = {t.point3.x, t.point3.y, t.point3.z},
                              .color    = u32_color_to_vec4(t.color3)};

    mesh.vertices.push_back(v1);
    mesh.vertices.push_back(v2);
    mesh.vertices.push_back(v3);

    const auto new_vertex_count = num_vertices + vertices_per_triangle;
    for (auto j = num_vertices; j < new_vertex_count; ++j) {
      mesh.indices.push_back(static_cast<Index>(num_vertices));
      ++num_vertices;
    }
  }

  return mesh;
}

[[nodiscard]] std::vector<afk::event::Event::Collision> CollisionSystem::get_current_collisions() {
  // make srue colliders are up to date
  this->syncronize_colliders();

  // clear and initialise temporary store
  this->temporary_collisions.clear();

  //// perform tests
  this->world->testCollision(this->collision_callback);

  //// empty the temporary store
  auto collisions = std::move(CollisionSystem::temporary_collisions);
  this->temporary_collisions = {};

  // return the collisions
  return collisions;
}

rp3d::PhysicsWorld *CollisionSystem::create_rp3d_physics_world() {
  // Instantiate the world
  auto physics_world = this->physics_common.createPhysicsWorld();
  // Disable gravity; ReactPhysics3D will only be used for collision detection, so gravity physics should not be applied
  physics_world->setIsGravityEnabled(false);

  // Turn off sleeping of collisions.
  // Then bodies are colliding but aren't really changing their state for a while, they will be put to 'sleep' to stop testing collisions.
  // Putting a body to sleep will make it no longer appear to be colliding externally.
  // Temporarily, we will be disabling this optimisation for simplicity's sake, so we can process every collision
  // @todo remove the need to turn off the sleeping optimisation in ReactPhysics3D
  physics_world->enableSleeping(false);

  // Set event listener used for firing collision events that occur in the ReactPhysics3D world
  physics_world->setEventListener(&this->event_listener);

  // turn on debug renderer so ReactPhysics3D creates render data
  // @todo turn off debug ReactPhysics3D data to be generated in ReactPhysics3D when debug rendering is not being used
  physics_world->setIsDebugRenderingEnabled(true);

  // Set all debug items to be displayed
  // @todo set which debug items to generate display data for in GUI
  physics_world->getDebugRenderer().setIsDebugItemDisplayed(
      rp3d::DebugRenderer::DebugItem::COLLIDER_AABB, true);
  physics_world->getDebugRenderer().setIsDebugItemDisplayed(
      rp3d::DebugRenderer::DebugItem::COLLIDER_BROADPHASE_AABB, true);
  physics_world->getDebugRenderer().setIsDebugItemDisplayed(
      rp3d::DebugRenderer::DebugItem::COLLISION_SHAPE, true);
  physics_world->getDebugRenderer().setIsDebugItemDisplayed(
      rp3d::DebugRenderer::DebugItem::CONTACT_POINT, true);
  physics_world->getDebugRenderer().setIsDebugItemDisplayed(
      rp3d::DebugRenderer::DebugItem::CONTACT_NORMAL, true);

  return physics_world;
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

void CollisionSystem::CollisionEventListener::onContact(
    const rp3d::CollisionCallback::CallbackData &callback_data) {
  // On collision event, there will be two colliders colliding
  // Iterate over all these pairs
  for (rp3d::uint p = 0; p < callback_data.getNbContactPairs(); p++) {

    // Get the contact pair
    const auto contact_pair = callback_data.getContactPair(p);

    auto &engine         = afk::Engine::get();

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

      auto &event_manager = engine.event_manager;

      // fire collision events
      // note that if a collision body is "sleeping" in reactphysics3d, a
      // collision event of type ContactStay will not fire at the moment,
      // "sleeping" is disabled treat contact enter and contact stay as "impulses"

      if (contact_pair.getEventType() == CollisionCallback::ContactPair::EventType::ContactStart ||
          contact_pair.getEventType() == CollisionCallback::ContactPair::EventType::ContactStay) {
        auto data = afk::event::Event::Collision{object1, object2, {}};

        afk_assert(contact_pair.getNbContactPoints() > 0,
                   "No contact points found on collision");

        data.contacts.reserve(contact_pair.getNbContactPoints());
        for (auto i = u32{0}; i < contact_pair.getNbContactPoints(); ++i) {
          const auto &contact_point = contact_pair.getContactPoint(i);
          const auto collider1_transform =
              contact_pair.getCollider1()->getLocalToWorldTransform();
          const auto collider2_transform =
              contact_pair.getCollider2()->getLocalToWorldTransform();
          const auto collider1_rp3d_point =
              collider1_transform * contact_point.getLocalPointOnCollider1();
          const auto collider2_rp3d_point =
              collider2_transform * contact_point.getLocalPointOnCollider2();

          const auto collider1_local_point =
              glm::vec3{collider1_rp3d_point.x, collider1_rp3d_point.y,
                        collider1_rp3d_point.z};
          const auto collider2_local_point =
              glm::vec3{collider2_rp3d_point.x, collider2_rp3d_point.y,
                        collider2_rp3d_point.z};
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

void CollisionSystem::CollisionCallback::onContact(const rp3d::CollisionCallback::CallbackData &callback_data) {

  // On collision event, there will be two colliders colliding
  // Iterate over all these pairs
  for (rp3d::uint p = 0; p < callback_data.getNbContactPairs(); p++) {

    // Get the contact pair
    const auto contact_pair = callback_data.getContactPair(p);

    auto &engine         = afk::Engine::get();

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

      // fire collision events
      // note that if a collision body is "sleeping" in reactphysics3d, a
      // collision event of type ContactStay will not fire at the moment,
      // "sleeping" is disabled treat contact enter and contact stay as "impulses"

      if (contact_pair.getEventType() == CollisionCallback::ContactPair::EventType::ContactStart ||
          contact_pair.getEventType() == CollisionCallback::ContactPair::EventType::ContactStay) {
        auto data = afk::event::Event::Collision{object1, object2, {}};

        afk_assert(contact_pair.getNbContactPoints() > 0,
                   "No contact points found on collision");

        data.contacts.reserve(contact_pair.getNbContactPoints());
        for (auto i = u32{0}; i < contact_pair.getNbContactPoints(); ++i) {
          const auto &contact_point = contact_pair.getContactPoint(i);
          const auto collider1_transform =
              contact_pair.getCollider1()->getLocalToWorldTransform();
          const auto collider2_transform =
              contact_pair.getCollider2()->getLocalToWorldTransform();
          const auto collider1_rp3d_point =
              collider1_transform * contact_point.getLocalPointOnCollider1();
          const auto collider2_rp3d_point =
              collider2_transform * contact_point.getLocalPointOnCollider2();

          const auto collider1_local_point =
              glm::vec3{collider1_rp3d_point.x, collider1_rp3d_point.y,
                        collider1_rp3d_point.z};
          const auto collider2_local_point =
              glm::vec3{collider2_rp3d_point.x, collider2_rp3d_point.y,
                        collider2_rp3d_point.z};
          const auto contact_normal = glm::vec3{contact_point.getWorldNormal().x,
                                                contact_point.getWorldNormal().y,
                                                contact_point.getWorldNormal().z};

          data.contacts.push_back(afk::event::Event::Collision::Contact{
              collider1_local_point, collider2_local_point, contact_normal,
              contact_point.getPenetrationDepth()});
        }

        // this is outright abuse of making the engine static and should not be encouraged
        // @todo pass data into the collision system neatly
        engine.collision_system.temporary_collisions.push_back(data);
      }
    }
  }
}

void CollisionSystem::Logger::log(rp3d::Logger::Level level, const std::string &physicsWorldName,
                                  rp3d::Logger::Category category, const std::string &message,
                                  const char *filename, int lineNumber) {
  if (level != rp3d::Logger::Level::Information) {
    afk::io::log << "[rp3d " << getLevelName(level) << "] " << message << "\n";
  }
}

rp3d::decimal CollisionSystem::RaycastCallback::notifyRaycastHit(const rp3d::RaycastInfo &info) {
  auto &afk = afk::Engine::get();
  // copy info
  afk.collision_system.camera_raycast_info.push_back(RaycastHitInfo{info.body, info.hitFraction});

  // return 1.0 so that rp3d knows to process more raycasts
  // this is necessary because the order of raycast hits is not guarenteed
  return 1.0f;
}