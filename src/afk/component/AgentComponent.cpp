#include "AgentComponent.hpp"

#include "afk/Afk.hpp"
using Afk::AI::AgentComponent;

AgentComponent::AgentComponent(GameObject e, const glm::vec3 &pos, dtCrowdAgentParams &p) {
  this->owning_entity = e;
  this->id = Afk::Engine::get().crowds.current_crowd().addAgent(&(pos.x), &p);
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

auto AgentComponent::update() -> void {
  auto &tf = Afk::Engine::get().registry.get<Afk::Transform>(this->owning_entity);
  auto agent_data = Afk::Engine::get().crowds.current_crowd().getAgent(this->id);
  tf.translation.x = agent_data->npos[0];
  tf.translation.y = agent_data->npos[1];
  tf.translation.z = agent_data->npos[2];
}
