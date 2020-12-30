#pragma once

#include <string>
#include <unordered_map>

#include "afk/ecs/component/Component.hpp"

namespace afk {
  namespace prefab {
    struct Prefab {
      std::string name = {};
      std::unordered_map<std::string, afk::ecs::component::Component> components = {};
    };
  }
}
