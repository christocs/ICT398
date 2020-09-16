#include "afk/ecs/Ecs.hpp"

#include "afk/io/Log.hpp"
#include "afk/io/Time.hpp"

using afk::ecs::Ecs;

auto Ecs::update() -> void {
  this->system_manager.update();
}

auto Ecs::initialize() -> void {
  afk::io::log << afk::io::get_date_time() << "ECS subsystem initialized\n";
}
