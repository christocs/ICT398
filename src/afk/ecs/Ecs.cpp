#include "afk/ecs/Ecs.hpp"

using afk::ecs::Ecs;

auto Ecs::update() -> void {
  this->system_manager.update();
}
