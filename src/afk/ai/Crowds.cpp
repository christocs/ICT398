#include "afk/ai/Crowds.hpp"

using Afk::AI::Crowds;

auto Crowds::current_crowd() -> dtCrowd & {
  return *(this->crowd);
}

auto Crowds::update(float dt_seconds) -> void {
  this->crowd->update(dt_seconds, nullptr);
}

auto Crowds::init(dtNavMesh *nav_mesh) -> void {
  this->crowd->init(1000,    // max agents
                    10.f,    // max agent radius
                    nav_mesh // nav mesh
  );
}
