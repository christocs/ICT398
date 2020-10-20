#pragma once

#include <variant>

#include "afk/ecs/component/ModelComponent.hpp"
#include "afk/ecs/component/ColliderComponent.hpp"
#include "afk/ecs/component/TransformComponent.hpp"
#include "afk/ecs/component/PhysicsComponent.hpp"
#include "afk/ecs/component/RocketLauncherComponent.hpp"

namespace afk {
  namespace ecs {
    namespace component {
      /** Variant of all component types. */
      using Component = std::variant<TransformComponent, ModelComponent, ColliderComponent, PhysicsComponent, RocketLauncherComponent>;
    }
  }
}
