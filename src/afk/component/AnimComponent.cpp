#include "afk/component/AnimComponent.hpp"
using afk::AnimComponent;

AnimComponent::AnimComponent(GameObject _owner, AnimComponent::Status _status,
                             const std::string &_name, float _time)
  : BaseComponent(_owner), status(_status), name(_name), time(_time) {}
