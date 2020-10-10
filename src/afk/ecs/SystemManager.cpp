#include "afk/ecs/SystemManager.hpp"

using afk::ecs::SystemManager;

auto SystemManager::register_display_update_system(const Update& update) -> void {
  this->display_update_systems.push_back(update);
}

auto SystemManager::register_update_system(const Update& update) -> void {
  this->display_update_systems.push_back(update);
}

auto SystemManager::display_update() const -> void {
  for (const auto &system_update : this->display_update_systems) {
    system_update();
  }
}

auto SystemManager::update() const -> void {
  for (const auto &system_update : this->update_systems) {
    system_update();
  }
}
