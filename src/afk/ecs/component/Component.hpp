#pragma once

#include <variant>

#include "afk/ecs/component/ModelsComponent.hpp"
#include "afk/ecs/component/ColliderComponent.hpp"
#include "afk/ecs/component/TransformComponent.hpp"
#include "afk/ecs/component/PhysicsComponent.hpp"

namespace afk {
  namespace ecs {
    namespace component {
      /** Variant of all component types. */
      using Component = std::variant<TransformComponent, ModelsComponent, ColliderComponent, PhysicsComponent>;
    }
  }
}
