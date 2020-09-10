#include "afk/ecs/system/RenderSystem.hpp"

#include "afk/Afk.hpp"
#include "afk/ecs/component/ModelComponent.hpp"
#include "afk/io/Log.hpp"
#include "afk/io/Path.hpp"

using afk::ecs::component::ModelComponent;
using afk::ecs::component::PositionComponent;
using afk::ecs::system::RenderSystem;

auto RenderSystem::update() -> void {
  auto &afk       = afk::Engine::get();
  auto &registry  = afk.ecs.registry;
  const auto view = registry.view<ModelComponent, PositionComponent>();

  for (const auto entity : view) {
    auto &model    = registry.get<ModelComponent>(entity);
    auto &position = registry.get<PositionComponent>(entity);

    auto shader = afk.renderer.get_shader_program(
        afk::io::get_resource_path("res/shader/default.prog"));
    auto transform = model.transform;
    transform.translation += position.position;
    afk.renderer.draw_model(model.model_handle, shader, transform);
  }
}
