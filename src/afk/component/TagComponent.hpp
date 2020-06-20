#pragma once

#include "afk/component/BaseComponent.hpp"

namespace Afk {
  class TagComponent : public BaseComponent {
  public:
    /**
     * Tag type
     * \todo change to enum class
     */
    enum Tag { TERRAIN, PLAYER, ENEMY };
    /**
     * Set of tags on this object
     * \todo change to unordered_set
     */
    std::set<Afk::TagComponent::Tag> tags = {};
  };
}
