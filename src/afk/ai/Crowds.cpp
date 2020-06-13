#include "afk/ai/Crowds.hpp"
#include <stdexcept>

using Afk::AI::Crowds;

auto Crowds::current_crowd() -> dtCrowd & {
  return *(this->crowd);
}

auto Crowds::update(float dt_seconds) -> void {
  this->crowd->update(dt_seconds, nullptr);
}

auto Crowds::init(NavMeshManager::nav_mesh_ptr nav_mesh) -> void {
  if (!this->crowd->init(100,     // max agents
                         10.f,    // max agent radius
                         nav_mesh.get() // nav mesh
                         )) {
    throw std::runtime_error{"Unable to init crowds"};
  }
}
