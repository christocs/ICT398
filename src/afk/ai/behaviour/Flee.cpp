#include "afk/ai/behaviour/Flee.hpp"

#include "afk/Afk.hpp"
#include "afk/physics/Transform.hpp"

using Afk::AI::Flee;

Flee::Flee(const GameObject &chase_target, float dist)
  : target(chase_target), desired_distance(dist) {}

auto Flee::update(const glm::vec3 &current_pos) -> glm::vec3 {
  const auto &chaser_pos =
      Afk::Engine::get().registry.get<Transform>(this->target).translation;

  // flee direction magnitude, should stop agents fleeing into walls
  const auto flee_magnitude = 10.f;
  const auto flee_dir = glm::normalize(current_pos - chaser_pos) * flee_magnitude;

  const auto goal_pos = current_pos + flee_dir;
  // don't flee past desired distance, but don't move towards if past max dist
  return glm::length(current_pos - chaser_pos) < desired_distance ? goal_pos : current_pos;
}
