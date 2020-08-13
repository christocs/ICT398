#pragma once
#include <string>

#include "afk/NumericTypes.hpp"
#include "afk/component/BaseComponent.hpp"
#include "afk/physics/Transform.hpp"

namespace afk {
  /**
   * Transformation component.
   */
  class TransformComponent : public BaseComponent, public physics::Transform {
  public:
    /**
     * Constructs a transformation component from the specified game object
     * owner.
     *
     * @param _owner The owner the constructed transform component.
     */
    TransformComponent(GameObject _owner);
  };
}
