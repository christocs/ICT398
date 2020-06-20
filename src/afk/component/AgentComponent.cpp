#include "AgentComponent.hpp"

#include <iostream>

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
#include "afk/ai/behaviour/Wander.hpp"
auto AgentComponent::wander(const glm::vec3 &target, float radius, float time) -> void {
  this->current_behaviour = Behaviour{new Afk::AI::Wander{target, radius, time}};
}

auto AgentComponent::update() -> void {
  if (this->current_behaviour == nullptr) {
    return;
  }
  auto &tf = Afk::Engine::get().registry.get<Afk::Transform>(this->owning_entity);
  auto agent = Afk::Engine::get().crowds.current_crowd().getAgent(this->id);
  if (!agent->active) {
    throw std::runtime_error{"Agent not active!"};
  }
  // Update our position to the current agent pos, if the physics body exists set that else set the transform
  if (Afk::Engine::get().registry.has<Afk::PhysicsBody>(this->owning_entity)) {
    auto &physics_body =
        Afk::Engine::get().registry.get<Afk::PhysicsBody>(this->owning_entity);
    physics_body.set_pos(glm::vec3{agent->npos[0], agent->npos[1], agent->npos[2]});
  } else {
    tf.translation.x = agent->npos[0];
    tf.translation.y = agent->npos[1];
    tf.translation.z = agent->npos[2];
  }
  // Update the agent goal
  auto next_pos = current_behaviour->update(tf.translation);
  Afk::Engine::get().crowds.request_move(this->id, next_pos);
}

AgentComponent::AgentComponent(AgentComponent &&rhs) {
  this->id            = rhs.id;
  this->owning_entity = rhs.owning_entity;
  rhs.id              = -1;
  rhs.owning_entity   = entt::null;
}

auto AgentComponent::operator=(AgentComponent &&rhs) -> AgentComponent & {
  this->id            = rhs.id;
  this->owning_entity = rhs.owning_entity;
  rhs.id              = -1;
  rhs.owning_entity   = entt::null;
  return *this;
}

auto AgentComponent::get_id() const -> Afk::AI::Crowds::AgentID {
  return id;
}
