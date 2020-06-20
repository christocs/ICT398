#pragma once

#include "afk/component/BaseComponent.hpp"

namespace Afk {
  class TagComponent : public BaseComponent {
  public:
    enum Tag { TERRAIN, PLAYER, ENEMY, PREY, DEATHZONE };

    using Tags = std::set<Afk::TagComponent::Tag>;

    Tags tags = {};
  };
}
