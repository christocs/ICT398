#pragma once

#include <entt/entt.hpp>

#include "afk/renderer/Renderer.hpp"

namespace Afk {
  /**
   * Queue models from ECS to the renderer for rendering
   */
  auto queue_models(entt::registry *registry, Afk::Renderer *renderer) -> void;
};
