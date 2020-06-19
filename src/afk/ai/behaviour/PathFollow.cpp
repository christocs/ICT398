#include "afk/ai/behaviour/PathFollow.hpp"

#include <numeric>

using Afk::AI::Path;
using Afk::AI::PathFollow;

PathFollow::PathFollow(const Path &p, const glm::vec3 &start, float accept_distance)
  : path(p), required_dist(accept_distance) {
  float closest_dist            = std::numeric_limits<float>::max();
  Path::size_type closest_index = 0;
  for (Path::size_type index = 0; index < this->path.size(); index++) {
    auto len = glm::length(this->path[index] - start);
    if (len < closest_dist) {
      closest_dist  = len;
      closest_index = index;
    }
  }
  this->path_index = closest_index;
}

auto PathFollow::update(const glm::vec3 &current_pos) -> glm::vec3 {
  if (glm::length(current_pos - this->path[this->path_index]) <= this->required_dist) {
    this->path_index = (this->path_index + 1) % this->path.size();
  }
  return this->path[this->path_index];
}
