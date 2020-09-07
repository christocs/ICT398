#pragma once

#include <set>

#include <entt/entt.hpp>

/// @cond DOXYGEN_IGNORE

namespace afk {
  class BaseComponent {
  public:
    BaseComponent() = default;
    BaseComponent(entt::entity _owner) : owning_entity(_owner) {}

    entt::entity owning_entity = {entt::null};
  };
}

/// @endcond
