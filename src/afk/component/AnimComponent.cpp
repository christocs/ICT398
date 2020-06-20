#include "afk/component/AnimComponent.hpp"
using Afk::AnimComponent;

AnimComponent::AnimComponent(GameObject owner) {
  this->owning_entity = owner;
}
