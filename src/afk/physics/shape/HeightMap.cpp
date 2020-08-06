#include "afk/physics/shape/HeightMap.hpp"

using afk::physics::shape::HeightMap;

auto HeightMap::at(Point p) const -> float {
  return this->heights.at(static_cast<size_t>(p.y * this->width + p.x));
}

auto HeightMap::operator[](Point p) -> float & {
  return this->heights[static_cast<size_t>(p.y * this->width + p.x)];
}
