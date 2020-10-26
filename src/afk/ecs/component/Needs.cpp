#include "afk/ecs/component/Needs.hpp"

#include <cmath>

using afk::ecs::component::Needs;

const string Needs::sit  = "sit";
const string Needs::kick = "kick";
const string Needs::grab = "grab";

Needs &Needs::operator-=(const Needs &right) {
  for (auto rkvp : right.need) {
    decltype(this->need)::iterator lkvpi;
    if ((lkvpi = this->need.find(rkvp.first)) != this->need.end()) {
      lkvpi->second = std::fmaxf(0, lkvpi->second - rkvp.second);
    }
  }
  return *this;
}

Needs &Needs::operator+=(const Needs &right) {
  for (auto rkvp : right.need) {
    decltype(this->need)::iterator lkvpi;
    if ((lkvpi = this->need.find(rkvp.first)) != this->need.end()) {
      lkvpi->second = std::fminf(1, lkvpi->second + rkvp.second);
    }
  }
  return *this;
}

float Needs::operator[](const string &index) {}
