#include "afk/ecs/system/AffordanceSystem.hpp"

#include <algorithm>

#include "afk/Engine.hpp"
#include "afk/ecs/component/AffordanceComponent.hpp"
#include "afk/ecs/component/NeedsComponent.hpp"
#include "afk/ecs/component/PositionComponent.hpp"
#include "afk/io/Log.hpp"
#include "afk/io/Path.hpp"
using afk::ecs::component::AffordanceComponent;
using afk::ecs::component::NeedsComponent;
using afk::ecs::component::PositionComponent;
using afk::ecs::system::AffordanceSystem;
auto AffordanceSystem::update() -> void {
  auto &afk       = afk::Engine::get();
  auto &registry  = afk.ecs.registry;
  const auto view = registry.view<NeedsComponent, PositionComponent>();
  for (const auto entity : view) {
    auto &needs          = registry.get<NeedsComponent>(entity);
    auto &pos            = registry.get<PositionComponent>(entity);
    entt::entity nearest = entt::null;
    auto greatest_need =
        std::max_element(needs.needs.need.begin(), needs.needs.need.end(),
                         [](auto a, auto b) { return a.second < b.second; });
    const auto affordables_view =
        registry.view<AffordanceComponent, PositionComponent>();
    for (const auto affordable_ents : affordables_view) {
    }
  }
}
