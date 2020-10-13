#include "afk/ecs/SystemManager.hpp"

using afk::ecs::SystemManager;

auto SystemManager::register_system(Update update) -> void {
  this->systems.push_back(update);
}

auto SystemManager::update() const -> void {
  for (const auto &system_update : this->systems) {
    system_update();
  }
}
