#pragma once

#include "afk/component/BaseComponent.hpp"

namespace Afk {
  class TagComponent : public BaseComponent {
  public:
    /**
     * Tag type
     * \todo change to enum class
     */
    enum Tag { TERRAIN, PLAYER, ENEMY, PREY, DEATHZONE };
    
    using Tags = std::set<Afk::TagComponent::Tag>;
    /**
     * Set of tags on this object
     * \todo change to unordered_set
     */
    Tags tags = {};
  };
}
