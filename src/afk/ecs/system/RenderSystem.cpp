#include "afk/ecs/system/RenderSystem.hpp"

#include "afk/Engine.hpp"
#include "afk/ecs/component/ModelsComponent.hpp"
#include "afk/ecs/component/TransformComponent.hpp"
#include "afk/io/Log.hpp"

using afk::ecs::component::ModelsComponent;
using afk::ecs::component::TransformComponent;
using afk::ecs::system::RenderSystem;

auto RenderSystem::update() -> void {
  auto &afk       = afk::Engine::get();
  auto &registry  = afk.ecs.registry;
  const auto view = registry.view<ModelsComponent, TransformComponent>();

  for (const auto entity : view) {
    auto &models     = registry.get<ModelsComponent>(entity);
    auto &parent_transform = registry.get<TransformComponent>(entity);

    auto shader = afk.renderer.get_shader_program(
        afk::io::get_resource_path("res/shader/default.prog"));
    for (const auto &model : models.models) {
      const auto transform = parent_transform.combined_transform_to_mat4(model.transform);
      afk.renderer.draw_model(model.model_handle, shader, transform);
    }
  }
}
