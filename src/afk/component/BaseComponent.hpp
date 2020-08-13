#pragma once

#include <set>

#include <entt/entt.hpp>

#include "afk/component/GameObject.hpp"

/// @cond DOXYGEN_IGNORE

namespace afk {
  class BaseComponent {
  public:
    BaseComponent() = default;
    BaseComponent(GameObject _owner) : owning_entity(_owner) {}

    GameObject owning_entity = {entt::null};
  };
}

/// @endcond
