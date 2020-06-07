#include "AgentComponent.hpp"

#include "afk/Afk.hpp"
using Afk::AI::AgentComponent;

AgentComponent::AgentComponent(GameObject e, Transform *t, dtCrowdAgentParams &p) {
  this->owning_entity = e;
  this->transform     = t;
  this->id = Afk::Engine::get().crowds.current_crowd().addAgent(&(t->translation.x), &p);
}
AgentComponent::~AgentComponent() {
  Afk::Engine::get().crowds.current_crowd().removeAgent(this->id);
}
auto AgentComponent::target(const glm::vec3 &target_pos) -> void {
  auto agent = Afk::Engine::get().crowds.current_crowd().getEditableAgent(this->id);
  agent->targetPos[0] = target_pos.x;
  agent->targetPos[1] = target_pos.y;
  agent->targetPos[2] = target_pos.z;
}
