#pragma once
#include <string>

#include "afk/component/BaseComponent.hpp"

namespace Afk {
  /**
   * Animations component
   */
  class AnimComponent : public BaseComponent {
  public:
    /**
     * Anim status
     */
    enum class Status { Paused, Playing, Stopped };

    AnimComponent(GameObject _owner, AnimComponent::Status _status,
                  const std::string &_name, float _time);
    /**
     * Current anim status
     */
    Status status = Status::Playing;
    /**
     * Animation name
     */
    std::string name = {};
    /**
     * Animation time
     */
    float time = {};
  };
}
