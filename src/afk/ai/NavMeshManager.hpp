#pragma once

#include <DetourNavMesh.h>
#include <Recast.h>
#include <filesystem>
#include <memory>

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "ChunkyTriMesh.hpp"
#include "afk/physics/Transform.hpp"
#include "afk/renderer/Mesh.hpp"
#include "afk/renderer/Model.hpp"

namespace Afk {
  namespace AI {
    class NavMeshManager {
    public:
      NavMeshManager() = default;

      using nav_mesh_ptr = std::shared_ptr<dtNavMesh>;

      bool initialise(const std::filesystem::path &file_path);

      bool bake();

      bool load(const std::filesystem::path &file_path);

      bool save(const std::filesystem::path &file_path);

      auto get_nav_mesh() -> nav_mesh_ptr;

      const Model &get_nav_mesh_model();

      const Model &get_height_field_model();

    private:
      nav_mesh_ptr nav_mesh = nav_mesh_ptr{dtAllocNavMesh(), &dtFreeNavMesh};

      Model height_field_model = {};

      Model nav_mesh_model = {};

      std::filesystem::path file_path_ = {};

      unsigned char *build_tile_nav_mesh(const int tile_x, const int tile_y,
                                         glm::vec3 bmin, glm::vec3 bmax,
                                         float cell_size, int tile_size,
                                         int &data_size, const std::shared_ptr<ChunkyTriMesh>& chunky_tri_mesh,
                                         const std::vector<float> &vertices,
                                         const std::vector<int> &triangles);

      static glm::vec3 transform_pos(const glm::vec3 &input, const Afk::Transform &transform);

      // vertex data is flat, order is [x][y][z]
      static void get_min_max_bounds(const std::vector<float> &vertices,
                                     glm::vec3 &min, glm::vec3 &max);

      void create_height_field_model(const rcHeightfield &height_field);

      void create_nav_mesh_model(const dtNavMesh &navMesh);

      static void process_nav_mesh_model_poly(const dtNavMesh &navMesh,
                                              Afk::Mesh &mesh, dtPolyRef ref);

      static const int NAVMESHSET_MAGIC = 'M' << 24 | 'S' << 16 | 'E' << 8 | 'T'; //'MSET';
      static const int NAVMESHSET_VERSION = 1;

      struct NavMeshSetHeader {
        int magic;
        int version;
        int numTiles;
        dtNavMeshParams params;
      };

      struct NavMeshTileHeader {
        dtTileRef tileRef;
        int dataSize;
      };

      enum PolyFlags {
        POLYFLAGS_NONE = 0,
        POLYFLAGS_WALK = 1, // Ability to walk (ground, grass, road)
        POLYFLAGS_SWIM = 2, // Ability to swim (water).
        POLYFLAGS_DOOR = 4, // Ability to move through doors.
        POLYFLAGS_JUMP = 8, // Ability to jump.
      };

      inline unsigned int nextPow2(unsigned int v);

      inline unsigned int ilog2(unsigned int v);
    };

  }
}
