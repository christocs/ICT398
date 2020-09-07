#include "afk/component/TransformComponent.hpp"

using afk::TransformComponent;

TransformComponent::TransformComponent(entt::entity _owner)
  : BaseComponent(_owner), Transform() {}
