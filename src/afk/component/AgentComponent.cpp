#include "AgentComponent.hpp"

#include "afk/Afk.hpp"

using Afk::AI::AgentComponent;

AgentComponent::AgentComponent(GameObject e, const glm::vec3 &pos, dtCrowdAgentParams &p) {
  this->owning_entity = e;

  auto nearest_pos = Afk::Engine::get().crowds.nearest_pos(pos, 10.f);
  if (nearest_pos.has_value()) {
    this->id = Afk::Engine::get().crowds.current_crowd().addAgent(&nearest_pos->x, &p);
  } else {
    throw std::runtime_error{"Could not find position for agent"};
  }
}
AgentComponent::~AgentComponent() {
  Afk::Engine::get().crowds.current_crowd().removeAgent(this->id);
}

#include "afk/ai/behaviour/Chase.hpp"
auto AgentComponent::chase(const GameObject &target, float max_dist) -> void {
  this->current_behaviour = Behaviour{new Afk::AI::Chase{target, max_dist}};
}
#include "afk/ai/behaviour/MoveTo.hpp"
auto AgentComponent::move_to(const glm::vec3 &target) -> void {
  this->current_behaviour = Behaviour{new Afk::AI::MoveTo{target}};
}
#include "afk/ai/behaviour/Flee.hpp"
auto AgentComponent::flee(const GameObject &target, float desired_dist) -> void {
  this->current_behaviour = Behaviour{new Afk::AI::Flee{target, desired_dist}};
}
#include "afk/ai/behaviour/PathFollow.hpp"
auto AgentComponent::path(const Afk::AI::Path &path, float min_dist) -> void {
  auto &my_pos = Afk::Engine::get().registry.get<Afk::Transform>(this->owning_entity);
  this->current_behaviour =
      Behaviour{new Afk::AI::PathFollow{path, my_pos.translation, min_dist}};
}

auto AgentComponent::update() -> void {
  if (this->current_behaviour == nullptr) {
    return;
  }
  auto &tf = Afk::Engine::get().registry.get<Afk::Transform>(this->owning_entity);
  auto agent = Afk::Engine::get().crowds.current_crowd().getAgent(this->id);
  if (!agent->active) {
    throw new std::runtime_error{"Agent not active!"};
  }
  // Update our position to the current agent pos
  tf.translation.x = agent->npos[0];
  tf.translation.y = agent->npos[1];
  tf.translation.z = agent->npos[2];
  // Update the agent goal
  auto next_pos = current_behaviour->update(tf.translation);
  Afk::Engine::get().crowds.request_move(this->id, next_pos);
}
