#pragma once

#include <set>

#include <entt/entt.hpp>

#include "afk/component/GameObject.hpp"

namespace Afk {
  class BaseComponent {
  public:
    BaseComponent() = default;
    BaseComponent(GameObject _owner) : owning_entity(_owner) {}

    GameObject owning_entity = {entt::null};
  };
}
