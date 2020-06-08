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

      const Model &get_height_field_model();

    private:
      dtNavMesh *nav_mesh = nullptr;

      Model height_field_model = {};

      Model nav_mesh_model = {};

      static glm::vec3 transform_pos(const glm::vec3 &input, const Afk::Transform &transform);

      static void get_min_max_bounds(const Afk::Mesh &mesh, glm::vec3 &min, glm::vec3 &max);

      void create_height_field_model(const rcHeightfield &heightField);

      void create_nav_mesh_model(const dtNavMesh &navMesh);

      static void process_nav_mesh_model_poly(const dtNavMesh &navMesh,
                                              Afk::Mesh &mesh, dtPolyRef ref);

      static const int NAVMESHSET_MAGIC = 'M'<<24 | 'S'<<16 | 'E'<<8 | 'T'; //'MSET';
      static const int NAVMESHSET_VERSION = 1;

      struct NavMeshSetHeader
      {
        int magic;
        int version;
        int numTiles;
        dtNavMeshParams params;
      };

      struct NavMeshTileHeader
      {
        dtTileRef tileRef;
        int dataSize;
      };
    };

  }
}
