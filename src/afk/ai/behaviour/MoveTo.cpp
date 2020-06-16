#include "afk/ai/behaviour/MoveTo.hpp"

#include "afk/Afk.hpp"
using Afk::AI::MoveTo;

MoveTo::MoveTo(const glm::vec3 &chase_target) : target(chase_target) {}

auto MoveTo::update([[maybe_unused]] const glm::vec3 &current_pos) -> glm::vec3 {
  return this->target;
}
