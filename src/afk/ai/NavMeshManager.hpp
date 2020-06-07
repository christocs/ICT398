#pragma once

#include <DetourNavMesh.h>
#include <filesystem>
#include <memory>

#include <glm/glm.hpp>

#include "Recast.h"
#include "afk/physics/Transform.hpp"
#include "afk/renderer/Mesh.hpp"
#include "afk/renderer/Model.hpp"

namespace Afk {
  namespace Ai {
    using AgentConfig = rcConfig;
    class NavMeshManager {
    public:
      NavMeshManager() = default;

      bool initialise(const std::filesystem::path& file_path, const Afk::Mesh& mesh, const Afk::Transform& transform);

      bool bake(const std::filesystem::path& file_path, const Afk::Mesh& mesh, const Afk::Transform& transform);

      bool load(const std::filesystem::path& file_path);

      bool save(const std::filesystem::path& file_path);

      const Model& get_height_field_model();

    private:
      dtNavMesh* navMesh = nullptr;

      Model heightFieldModel = {};

      dtNavMesh* get_nav_mesh();

      void get_min_max_bounds(const Afk::Mesh& mesh, const Afk::Transform& transform, glm::vec3& min, glm::vec3& max);

      static glm::vec3 transform_pos(const glm::vec3& input, const Afk::Transform& transform);

      void create_height_field_model(const rcHeightfield& heightField);
    };

  }
}

