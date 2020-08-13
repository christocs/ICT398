#include "afk/component/TransformComponent.hpp"

using afk::GameObject;
using afk::TransformComponent;

TransformComponent::TransformComponent(GameObject _owner)
  : BaseComponent(_owner), Transform() {}
