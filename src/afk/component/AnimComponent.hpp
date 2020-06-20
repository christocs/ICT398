#pragma once
#include <string>

#include "afk/component/BaseComponent.hpp"

namespace Afk {
  /**
   * Animations component
   */
  class AnimComponent : public BaseComponent {
    /**
     * Anim status
     */
    enum class Status { Paused, Playing, Stopped };
    /**
     * Current anim status
     */
    Status status = Status::Playing;
    /**
     * Animation name
     */
    std::string name;
    /**
     * Animation time
     */
    float time;
  }
}
