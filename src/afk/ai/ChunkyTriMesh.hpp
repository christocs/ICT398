#pragma once

#include <memory>

namespace Afk {
  namespace AI {

    /**
     * Tidied up code from recast demo
     */
    class ChunkyTriMesh {
    public:
      struct ChunkyTriMeshNode {
        float bmin[2];
        float bmax[2];
        int i;
        int n;
      };

      ChunkyTriMesh() = default;

      /// Creates partitioned triangle mesh (AABB tree),
      /// where each node contains at max trisPerChunk triangles.
      bool init(const float *verts, const int *tris, int ntris,
                int trisPerChunk, ChunkyTriMesh *cm);

      /// Returns the chunk indices which overlap the input rectable.
      int get_chunks_overlapping_rect(float *bmin, float *bmax, int *ids, int maxIds) const;

      std::shared_ptr<ChunkyTriMeshNode> nodes = nullptr;
      int nnodes                               = 0;
      std::unique_ptr<int[]> tris              = nullptr;
      int ntris                                = 0;
      int maxTrisPerChunk                      = 0;

    private:
      // Explicitly disabled copy constructor and copy assignment operator.
      ChunkyTriMesh(const ChunkyTriMesh &);
      ChunkyTriMesh &operator=(const ChunkyTriMesh &);
    };

    struct BoundsItem {
      float bmin[2];
      float bmax[2];
      int i;
    };

    [[maybe_unused]] static int compare_item_x(const void *va, const void *vb);

    [[maybe_unused]] static int compare_item_y(const void *va, const void *vb);

    [[maybe_unused]] static void calc_extends(const BoundsItem *items, const int imin,
                                              const int imax, float *bmin, float *bmax);

    int longest_axis(float x, float y);

    [[maybe_unused]] static void
    subdivide(BoundsItem *items, int nitems, int imin, int imax, int trisPerChunk,
              int &curNode, ChunkyTriMesh::ChunkyTriMeshNode *nodes,
              const int maxNodes, int &curTri, int *outTris, const int *inTris);

    inline bool check_overlap_rect(const float *amin, const float *amax,
                                   const float *bmin, const float *bmax);
  }
}
