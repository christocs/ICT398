#include "afk/ai/behaviour/Wander.hpp"

#include <ctime>
#include <random>

using Afk::AI::Wander;

Wander::Wander(const glm::vec3 &wander_target, float wander_range)
  : center(wander_target), last_wander(wander_target), range(wander_range) {}

auto Wander::update(const glm::vec3 &current_position) -> glm::vec3 {
  static std::mt19937 rng{static_cast<decltype(rng)::result_type>(std::time(nullptr))};

  constexpr auto wander_scale = 2.f;
  const auto wander_x = (rng() / static_cast<float>(decltype(rng)::max())) * wander_scale;
  const auto wander_z = (rng() / static_cast<float>(decltype(rng)::max())) * wander_scale;
  auto next_target = glm::vec3{last_wander.x + wander_x, current_position.y,
                               last_wander.z + wander_z};

  const auto center_dist = next_target - this->center;
  if (glm::length(center_dist) > this->range) {
    const auto within_bounds = glm::normalize(center_dist) * this->range;
    next_target              = this->center + within_bounds;
  }
  last_wander = current_position;
  return next_target;
}
