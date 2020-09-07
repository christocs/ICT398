#include "afk/component/AnimComponent.hpp"

#include "afk/NumericTypes.hpp"

using afk::AnimComponent;

AnimComponent::AnimComponent(entt::entity _owner, AnimComponent::Status _status,
                             const std::string &_name, f32 _time)
  : BaseComponent(_owner), status(_status), name(_name), time(_time) {}
