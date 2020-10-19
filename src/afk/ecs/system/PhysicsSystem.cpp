#include "afk/ecs/system/PhysicsSystem.hpp"

#include "afk/Engine.hpp"
#include "afk/debug/Assert.hpp"
#include "afk/ecs/component/ColliderComponent.hpp"
#include "afk/ecs/component/PhysicsComponent.hpp"
#include "afk/ecs/component/TransformComponent.hpp"
#include "afk/io/Log.hpp"

using afk::ecs::component::ColliderComponent;
using afk::ecs::component::PhysicsComponent;
using afk::ecs::component::TransformComponent;
using afk::ecs::system::PhysicsSystem;
using afk::event::Event;

auto PhysicsSystem::update() -> void {
  auto &afk           = afk::Engine::get();
  auto &registry      = afk.ecs.registry;
  auto &event_manager = afk.event_manager;
  const auto view =
      registry.view<ColliderComponent, PhysicsComponent, TransformComponent>();

  for (const auto entity : view) {
  }
}

auto PhysicsSystem::collision_resolution_callback(Event event) -> void {
  afk_assert(event.type == afk::event::Event::Type::Collision,
             "event type was not 'Collision'");
}