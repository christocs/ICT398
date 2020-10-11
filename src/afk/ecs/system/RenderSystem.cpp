#include "afk/ecs/system/RenderSystem.hpp"

#include "afk/Engine.hpp"
#include "afk/ecs/component/ModelComponent.hpp"
#include "afk/ecs/component/TransformComponent.hpp"
#include "afk/io/Log.hpp"

using afk::ecs::component::ModelComponent;
using afk::ecs::component::TransformComponent;
using afk::ecs::system::RenderSystem;

auto RenderSystem::update() -> void {
  auto &afk       = afk::Engine::get();
  auto &registry  = afk.ecs.registry;
  const auto view = registry.view<ModelComponent, TransformComponent>();

  for (const auto entity : view) {
    auto &model     = registry.get<ModelComponent>(entity);
    auto &transform = registry.get<TransformComponent>(entity);
    auto newT = transform;

    auto shader = afk.renderer.get_shader_program(
        afk::io::get_resource_path("res/shader/default.prog"));
    afk.renderer.draw_model(model.model_handle, shader, transform);
  }
}
