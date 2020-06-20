#include "afk/ai/ChunkyTriMesh.hpp"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

using Afk::AI::ChunkyTriMesh;

bool ChunkyTriMesh::init(const float *verts, const int *tris, int ntris,
                         int trisPerChunk, ChunkyTriMesh *cm) {
  int nchunks = (ntris + trisPerChunk - 1) / trisPerChunk;

  this->nodes = std::shared_ptr<ChunkyTriMeshNode>(
      new ChunkyTriMeshNode[nchunks * 4], std::default_delete<ChunkyTriMeshNode[]>());
  if (!this->nodes)
    return false;

  this->tris = std::make_unique<int[]>(ntris * 3);
  if (!this->tris)
    return false;

  this->ntris = ntris;

  // Build tree
  auto *items = new BoundsItem[ntris];

  for (int i = 0; i < ntris; i++) {
    const int *t   = &tris[i * 3];
    BoundsItem &it = items[i];
    it.i           = i;
    // Calc triangle XZ bounds.
    it.bmin[0] = it.bmax[0] = verts[t[0] * 3 + 0];
    it.bmin[1] = it.bmax[1] = verts[t[0] * 3 + 2];
    for (int j = 1; j < 3; ++j) {
      const float *v = &verts[t[j] * 3];
      if (v[0] < it.bmin[0])
        it.bmin[0] = v[0];
      if (v[2] < it.bmin[1])
        it.bmin[1] = v[2];

      if (v[0] > it.bmax[0])
        it.bmax[0] = v[0];
      if (v[2] > it.bmax[1])
        it.bmax[1] = v[2];
    }
  }

  int curTri  = 0;
  int curNode = 0;
  Afk::AI::subdivide(items, ntris, 0, ntris, trisPerChunk, curNode,
                     this->nodes.get(), nchunks * 4, curTri, this->tris.get(), tris);

  delete[] items;

  this->nnodes = curNode;

  // Calc max tris per node.
  this->maxTrisPerChunk = 0;
  for (int i = 0; i < this->nnodes; ++i) {
    ChunkyTriMesh::ChunkyTriMeshNode &node = this->nodes.get()[i];
    const bool isLeaf                      = node.i >= 0;
    if (!isLeaf) {
      continue;
    }
    if (node.n > this->maxTrisPerChunk) {
      this->maxTrisPerChunk = node.n;
    }
  }

  return true;
}

int ChunkyTriMesh::get_chunks_overlapping_rect(float *bmin, float *bmax,
                                               int *ids, const int maxIds) const {
  // Traverse tree
  int i = 0;
  int n = 0;
  while (i < this->nnodes) {
    const ChunkyTriMeshNode *node = &this->nodes.get()[i];
    const bool overlap = check_overlap_rect(bmin, bmax, node->bmin, node->bmax);
    const bool isLeafNode = node->i >= 0;

    if (isLeafNode && overlap) {
      if (n < maxIds) {
        ids[n] = i;
        n++;
      }
    }

    if (overlap || isLeafNode)
      i++;
    else {
      const int escapeIndex = -node->i;
      i += escapeIndex;
    }
  }

  return n;
}

static int Afk::AI::compare_item_x(const void *va, const void *vb) {
  const BoundsItem *a = (const BoundsItem *)va;
  const BoundsItem *b = (const BoundsItem *)vb;
  if (a->bmin[0] < b->bmin[0])
    return -1;
  if (a->bmin[0] > b->bmin[0])
    return 1;
  return 0;
}

static int Afk::AI::compare_item_y(const void *va, const void *vb) {
  const BoundsItem *a = (const BoundsItem *)va;
  const BoundsItem *b = (const BoundsItem *)vb;
  if (a->bmin[1] < b->bmin[1])
    return -1;
  if (a->bmin[1] > b->bmin[1])
    return 1;
  return 0;
}

static void Afk::AI::calc_extends(const BoundsItem *items, const int imin,
                                  const int imax, float *bmin, float *bmax) {
  bmin[0] = items[imin].bmin[0];
  bmin[1] = items[imin].bmin[1];

  bmax[0] = items[imin].bmax[0];
  bmax[1] = items[imin].bmax[1];

  for (int i = imin + 1; i < imax; ++i) {
    const BoundsItem &it = items[i];
    if (it.bmin[0] < bmin[0])
      bmin[0] = it.bmin[0];
    if (it.bmin[1] < bmin[1])
      bmin[1] = it.bmin[1];

    if (it.bmax[0] > bmax[0])
      bmax[0] = it.bmax[0];
    if (it.bmax[1] > bmax[1])
      bmax[1] = it.bmax[1];
  }
}

inline int Afk::AI::longest_axis(float x, float y) {
  return y > x ? 1 : 0;
}

static void Afk::AI::subdivide(BoundsItem *items, int nitems, int imin,
                               int imax, int trisPerChunk, int &curNode,
                               ChunkyTriMesh::ChunkyTriMeshNode *nodes, const int maxNodes,
                               int &curTri, int *outTris, const int *inTris) {
  int inum = imax - imin;
  int icur = curNode;

  if (curNode > maxNodes)
    return;

  ChunkyTriMesh::ChunkyTriMeshNode &node = nodes[curNode++];

  if (inum <= trisPerChunk) {
    // Leaf
    Afk::AI::calc_extends(items, imin, imax, node.bmin, node.bmax);

    // Copy triangles.
    node.i = curTri;
    node.n = inum;

    for (int i = imin; i < imax; ++i) {
      const int *src = &inTris[items[i].i * 3];
      int *dst       = &outTris[curTri * 3];
      curTri++;
      dst[0] = src[0];
      dst[1] = src[1];
      dst[2] = src[2];
    }
  } else {
    // Split
    Afk::AI::calc_extends(items, imin, imax, node.bmin, node.bmax);

    int axis = longest_axis(node.bmax[0] - node.bmin[0], node.bmax[1] - node.bmin[1]);

    if (axis == 0) {
      // Sort along x-axis
      qsort(items + imin, static_cast<size_t>(inum), sizeof(BoundsItem), Afk::AI::compare_item_x);
    } else if (axis == 1) {
      // Sort along y-axis
      qsort(items + imin, static_cast<size_t>(inum), sizeof(BoundsItem), Afk::AI::compare_item_y);
    }

    int isplit = imin + inum / 2;

    // Left
    subdivide(items, nitems, imin, isplit, trisPerChunk, curNode, nodes,
              maxNodes, curTri, outTris, inTris);
    // Right
    subdivide(items, nitems, isplit, imax, trisPerChunk, curNode, nodes,
              maxNodes, curTri, outTris, inTris);

    int iescape = curNode - icur;
    // Negative index means escape.
    node.i = -iescape;
  }
}

inline bool Afk::AI::check_overlap_rect(const float *amin, const float *amax,
                                        const float *bmin, const float *bmax) {
  bool overlap = true;
  overlap      = (amin[0] > bmax[0] || amax[0] < bmin[0]) ? false : overlap;
  overlap      = (amin[1] > bmax[1] || amax[1] < bmin[1]) ? false : overlap;
  return overlap;
}
