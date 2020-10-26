#include "afk/ecs/component/Needs.hpp"
using afk::ecs::component::Needs;

Needs &Needs::operator-=(const Needs &right) {
  this->eat -= right.eat;
  this->sit -= right.sit;
  this->kick -= right.kick;
  return *this;
}
