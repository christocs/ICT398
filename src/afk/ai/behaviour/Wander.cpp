#include "afk/ai/behaviour/Wander.hpp"

#include <cmath>
#include <ctime>
#include <random>

#include "afk/Afk.hpp"

using Afk::AI::Wander;

Wander::Wander(const glm::vec3 &wander_target, float wander_range, float wander_time)
  : center(wander_target), last_wander(wander_target),
    wander_wait_time(wander_time), range(wander_range) {
  this->last_wander_change_time = Afk::Engine::get().get_time();
}

auto Wander::update(const glm::vec3 &current_position) -> glm::vec3 {
  static std::mt19937 rng{static_cast<decltype(rng)::result_type>(std::time(nullptr))};
  auto current_time = Afk::Engine::get().get_time();
  if ((current_time - this->last_wander_change_time) < this->wander_wait_time) {
    return this->last_wander;
  }
  this->last_wander_change_time = current_time;
  double a = rng() / static_cast<double>(decltype(rng)::max());
  double r = this->range * std::sqrt(rng() / static_cast<double>(decltype(rng)::max()));
  double x          = r * cos(a);
  double z          = r * sin(a);
  this->last_wander = glm::vec3{x, current_position.y, z};
  // constexpr auto wander_scale   = 5.f;
  // const auto wander_x = (rng() / static_cast<float>(decltype(rng)::max())) * wander_scale;
  // const auto wander_z = (rng() / static_cast<float>(decltype(rng)::max())) * wander_scale;
  // auto next_target = glm::vec3{last_wander.x + wander_x, current_position.y,
  //                              last_wander.z + wander_z};

  // const auto center_dist = next_target - this->center;
  // if (glm::length(center_dist) > this->range) {
  //   const auto within_bounds = glm::normalize(center_dist) * this->range;
  //   next_target              = this->center + within_bounds;
  // }
  return this->last_wander;
}
