#pragma once

#include <variant>

#include "afk/ecs/component/ModelComponent.hpp"
#include "afk/ecs/component/PositionComponent.hpp"
#include "afk/ecs/component/VelocityComponent.hpp"
#include "afk/ecs/component/ColliderComponent.hpp"

namespace afk {
  namespace ecs {
    namespace component {
      /** Variant of all component types. */
      using Component = std::variant<ModelComponent, PositionComponent, VelocityComponent, ColliderComponent>;
    }
  }
}
