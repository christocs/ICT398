#include "afk/ecs/component/Needs.hpp"

#include <cmath>

using afk::ecs::component::Needs;

Needs &Needs::operator-=(const Needs &right) {
  this->eat  = std::fminf(0, this->eat - right.eat);
  this->sit  = std::fminf(0, this->sit - right.sit);
  this->kick = std::fminf(0, this->kick - right.kick);
  return *this;
}

Needs &Needs::operator+=(const Needs &right) {
  this->eat  = std::fmaxf(1, this->eat + right.eat);
  this->sit  = std::fminf(1, this->sit + right.sit);
  this->kick = std::fminf(1, this->kick + right.kick);
  return *this;
}
