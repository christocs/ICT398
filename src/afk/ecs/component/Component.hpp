#pragma once

#include <variant>

#include "afk/ecs/component/ModelComponent.hpp"
#include "afk/ecs/component/PositionComponent.hpp"
#include "afk/ecs/component/VelocityComponent.hpp"

namespace afk {
  namespace ecs {
    namespace component {
      /** Variaint of all component types. */
      using Component = std::variant<ModelComponent, PositionComponent, VelocityComponent>;
    }
  }
}
