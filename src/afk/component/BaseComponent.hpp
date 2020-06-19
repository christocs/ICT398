#pragma once

#include <set>
#include <entt/entt.hpp>

#include "afk/component/GameObject.hpp"

namespace Afk {
  class BaseComponent {
  public:
    GameObject owning_entity = {entt::null};
  };
}
