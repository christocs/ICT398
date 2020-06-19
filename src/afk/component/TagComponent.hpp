#pragma once

#include "afk/component/BaseComponent.hpp"

namespace Afk {
  class TagComponent : public BaseComponent {
  public:
    enum Tag { TERRAIN, PLAYER, ENEMY };

    std::set<Afk::TagComponent::Tag> tags = {};
  };
}
