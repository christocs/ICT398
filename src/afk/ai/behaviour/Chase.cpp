#include "afk/ai/behaviour/Chase.hpp"

#include "afk/Afk.hpp"
#include "afk/physics/Transform.hpp"

using Afk::AI::Chase;

Chase::Chase(const GameObject &chase_target, float max_distance)
  : target(chase_target), max_dist(max_distance) {}

auto Chase::update(const glm::vec3 &current_pos) -> glm::vec3 {
  const auto &goal_pos =
      Afk::Engine::get().registry.get<Transform>(this->target).translation;
  // don't chase past max distance
  return glm::length(goal_pos - current_pos) < max_dist ? goal_pos : current_pos;
}
