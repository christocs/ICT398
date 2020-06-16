#include "afk/ai/Crowds.hpp"

#include <stdexcept>

#include <glm/glm.hpp>

using Afk::AI::Crowds;

auto Crowds::current_crowd() -> dtCrowd & {
  return *(this->crowd);
}

auto Crowds::update(float dt_seconds) -> void {
  this->crowd->update(dt_seconds, nullptr);
}

auto Crowds::nearest_pos(glm::vec3 pos, float search_dist) -> std::optional<glm::vec3> {
  auto query             = this->crowd->getNavMeshQuery();
  const auto extents     = glm::vec3{search_dist, search_dist, search_dist};
  const auto filter      = dtQueryFilter{};
  dtPolyRef nearest_poly = {};
  auto nearest_pos       = glm::vec3{0, 0, 0};
  auto query_result      = query->findNearestPoly(&pos.x, &extents.x, &filter,
                                             &nearest_poly, &nearest_pos.x);
  if (nearest_poly == 0) {
    return std::nullopt;
  }
  return std::optional{nearest_pos};
}

auto Crowds::init(NavMeshManager::nav_mesh_ptr nav_mesh) -> void {
  if (!this->crowd->init(100,           // max agents
                         10.f,          // max agent radius
                         nav_mesh.get() // nav mesh
                         )) {
    throw std::runtime_error{"Unable to init crowds"};
  }
}
