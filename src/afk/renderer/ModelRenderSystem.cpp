#include "afk/renderer/ModelRenderSystem.hpp"

#include "afk/io/ModelSource.hpp"
#include "afk/physics/Transform.hpp"

auto Afk::queue_models(entt::registry *registry, Afk::Renderer *renderer) -> void {
  auto render_view = registry->view<Afk::Transform, Afk::ModelSource>();

  for (const auto entity : render_view) {
    const auto& model_component = render_view.get<Afk::ModelSource>(entity);
    const auto& model_transform = render_view.get<Afk::Transform>(entity);
    renderer->queue_draw({model_component.name, model_component.shader_program_path, model_transform});
  }
}
