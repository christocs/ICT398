#pragma once

#include <DetourNavMesh.h>
#include <Recast.h>
#include <filesystem>
#include <memory>

#include <glm/glm.hpp>

#include "afk/physics/Transform.hpp"
#include "afk/renderer/Mesh.hpp"
#include "afk/renderer/Model.hpp"

namespace Afk {
  namespace AI {
    using AgentConfig = rcConfig;
    class NavMeshManager {
    public:
      NavMeshManager() = default;

      bool initialise(const std::filesystem::path &file_path,
                      const Afk::Mesh &mesh, const Afk::Transform &transform);

      bool bake(const std::filesystem::path &file_path, const Afk::Mesh &mesh,
                const Afk::Transform &transform);

      bool load(const std::filesystem::path &file_path);

      bool save(const std::filesystem::path &file_path);

      auto get_nav_mesh() -> dtNavMesh *;

      const Model &get_nav_mesh_model();

    private:
      dtNavMesh *nav_mesh = nullptr;

      Model height_field_model = {};

      Model nav_mesh_model = {};

      static glm::vec3 transform_pos(const glm::vec3 &input, const Afk::Transform &transform);

      void get_min_max_bounds(const Afk::Mesh &mesh, const Afk::Transform &transform,
                              glm::vec3 &min, glm::vec3 &max);

      void create_height_field_model(const rcHeightfield &heightField);

      void create_nav_mesh_model(const dtNavMesh &navMesh);

      static void process_nav_mesh_model_poly(const dtNavMesh &navMesh,
                                              Afk::Mesh &mesh, dtPolyRef ref);
    };

  }
}
