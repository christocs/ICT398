#include "afk/ai/DifficultyManager.hpp"

#include "afk/Afk.hpp"

#include "afk/component/AgentComponent.hpp"

using Afk::AI::DifficultyManager;

auto DifficultyManager::init(const Difficulty& diff) ->  void {
  set_difficulty(diff);
}

auto DifficultyManager::set_difficulty(const Difficulty& diff) -> void
{
  difficulty = diff;

  auto agent_entity_view =
      Afk::Engine::get().registry.view<Afk::AI::AgentComponent>();

  for (const auto &entity : agent_entity_view) {
    const auto &agent = Afk::Engine::get().registry.get<Afk::AI::AgentComponent>(entity);
    const auto agent_id = agent.get_id();
    const auto &dt_agent = Afk::Engine::get().crowds.current_crowd().getAgent(agent_id);
    if (dt_agent) {
      auto agent_params = dt_agent->params; // get a copy
      if (difficulty == Difficulty::EASY) {
        agent_params.maxSpeed        = 1.0f;
        agent_params.maxAcceleration = 1.0f;
      } else if (difficulty == Difficulty::NORMAL) {
        agent_params.maxSpeed        = 3.0f;
        agent_params.maxAcceleration = 3.0f;
      } else {
        agent_params.maxSpeed        = 10.0f;
        agent_params.maxAcceleration = 10.0f;
      }

      Afk::Engine::get().crowds.current_crowd().updateAgentParameters(agent_id, &agent_params);
    }
  }
}
DifficultyManager::Difficulty DifficultyManager::get_difficulty() {
  return difficulty;
}
