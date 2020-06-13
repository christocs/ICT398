#include "NavMeshManager.hpp"

#include <fstream>

#include <afk/debug/Assert.hpp>
#include <glm/gtc/type_ptr.inl>

#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h"
#include "DetourNavMeshQuery.h"
#include "afk/io/Log.hpp"

using Afk::AI::NavMeshManager;

bool NavMeshManager::initialise(const std::filesystem::path &file_path,
                                const Afk::Mesh &mesh, const Afk::Transform &transform) {
  if (!this->load(file_path)) {
    if (this->bake(file_path, mesh, transform)) {
      if (!this->save(file_path)) {
        Afk::Io::log << "Failed to bake nav mesh" << '\n';
        return false;
      } else {
        std::cout << "file bake saved" << std::endl;
      }
    }
  } else {
    std::cout << "file loaded" << std::endl;
  }
  return true;
}

bool NavMeshManager::bake(const std::filesystem::path &file_path,
                          const Afk::Mesh &mesh, const Afk::Transform &transform) {
  // use default values from demo
  rcConfig config                 = {};
  config.cs                       = 0.3f;  // cell size
  config.ch                       = 0.2f;  // cell height
  config.walkableSlopeAngle       = 45.0f; // 45
  const auto agentHeight          = 2.0f;  // 2.0
  config.walkableHeight           = (int)ceilf(agentHeight / config.ch);
  const auto agentMaxClimb        = 0.9f;
  config.walkableClimb            = (int)floorf(agentMaxClimb / config.ch);
  const auto agentRadius          = 0.6f;
  config.walkableRadius           = (int)ceilf(agentRadius / config.cs);
  const auto maxEdgeLength        = 12.0f;
  config.maxEdgeLen               = (int)(maxEdgeLength / config.cs);
  config.maxSimplificationError   = 1.0f;
  config.minRegionArea            = (int)(8 * 8);
  config.mergeRegionArea          = (int)(20 * 20);
  config.maxVertsPerPoly          = 6;
  const auto detailSampleDistance = 6;
  config.detailSampleDist         = detailSampleDistance * config.cs;
  const auto detailSampleMaxError = 1.0f;
  config.detailSampleMaxError     = config.ch * detailSampleMaxError;

  glm::vec3 bmin = {};
  glm::vec3 bmax = {};
  NavMeshManager::get_min_max_bounds(mesh, bmin, bmax);
  bmin           = NavMeshManager::transform_pos(bmin, transform);
  bmax           = NavMeshManager::transform_pos(bmax, transform);
  config.bmin[0] = bmin.x;
  config.bmin[1] = bmin.y;
  config.bmin[2] = bmin.z;
  config.bmax[0] = bmax.x;
  config.bmax[1] = bmax.y;
  config.bmax[2] = bmax.z;

  rcCalcGridSize(config.bmin, config.bmax, config.cs, &config.width, &config.height);

  const auto heightField = rcAllocHeightfield();
  afk_assert(heightField, "Could not allocate height field");

  rcContext context = {};
  auto tempStatus =
      rcCreateHeightfield(&context, *heightField, config.width, config.height,
                                   config.bmin, config.bmax, config.cs, config.ch);
  afk_assert(tempStatus, "Could not create height field");

  const size_t nvertices = mesh.vertices.size();

  const std::unique_ptr<float[]> vertices(new float[nvertices * 3]);
  const auto &meshVertices = mesh.vertices;
  size_t vertexCount       = 0;
  for (const auto &meshVertex : meshVertices) {
    const auto pos = NavMeshManager::transform_pos(meshVertex.position, transform);
    vertices[vertexCount++] = pos.x;
    vertices[vertexCount++] = pos.y;
    vertices[vertexCount++] = pos.z;
  }

  const size_t ntriangles = mesh.indices.size() / 3; // should be an integer

  // copy indicies into array of ints (afk stores them as unsigned int)
  const auto &indices = mesh.indices;
  const std::unique_ptr<int[]> triangles(new int[mesh.indices.size()]);
  // flip order of indices
  for (size_t i = 0; i < indices.size();) {
    triangles[i]     = indices[i];
    triangles[i + 1] = indices[i + 2];
    triangles[i + 2] = indices[i + 1];
    i += 3;
  }

  // Find triangles which are walkable based on their slope and rasterize them. If your input data is multiple meshes, you can transform them here, calculate the are type for each of the meshes and rasterize them.
  const std::unique_ptr<unsigned char[]> areas(new unsigned char[ntriangles]);
  memset(areas.get(), 0, ntriangles * sizeof(unsigned char));

  rcMarkWalkableTriangles(&context, config.walkableSlopeAngle, vertices.get(),
                          nvertices, triangles.get(), ntriangles, areas.get());

  tempStatus = rcRasterizeTriangles(&context, vertices.get(), nvertices,
                                    triangles.get(), areas.get(), ntriangles,
                                    *heightField, config.walkableClimb);
  afk_assert(tempStatus, "Could not rasterize triangles");

  rcFilterLowHangingWalkableObstacles(&context, config.walkableClimb, *heightField);
  rcFilterLedgeSpans(&context, config.walkableHeight, config.walkableClimb, *heightField);
  rcFilterWalkableLowHeightSpans(&context, config.walkableHeight, *heightField);

  this->create_height_field_model(*heightField);

  int spanCount = 0;
  for (int y = 0; y < heightField->height; ++y) {
    for (int x = 0; x < heightField->width; ++x) {
      for (rcSpan *s = heightField->spans[x + y * heightField->width]; s; s = s->next) {
        if (s->area != RC_NULL_AREA)
          spanCount++;
      }
    }
  }
  afk_assert(spanCount, "no spans found");

  // compact version
  auto compactHeightField = rcAllocCompactHeightfield();
  afk_assert(compactHeightField, "Could not allocate compact height field");

  tempStatus = rcBuildCompactHeightfield(&context, config.walkableHeight, config.walkableClimb,
                                         *heightField, *compactHeightField);
  afk_assert(tempStatus, "Could not build compact height field");

  rcFreeHeightField(heightField);

  // Erode the walkable area by agent radius.
  tempStatus = rcErodeWalkableArea(&context, config.walkableRadius, *compactHeightField);
  afk_assert(tempStatus, "Could not erode walkable area");

  // monotone
  tempStatus = rcBuildRegionsMonotone(&context, *compactHeightField, 0,
                                      config.minRegionArea, config.mergeRegionArea);
  afk_assert(tempStatus, "Could not build monotone regions");

  //
  // Step 5. Trace and simplify region contours.
  //
  auto contours = rcAllocContourSet();
  afk_assert(contours, "Could not allocate contours set");

  tempStatus = rcBuildContours(&context, *compactHeightField, config.maxSimplificationError,
                               config.maxEdgeLen, *contours);
  afk_assert(tempStatus, "Could not build contours");
  afk_assert(contours->nconts, "No contours generated");

  auto polyMesh = rcAllocPolyMesh();
  afk_assert(polyMesh, "Could not allocate poly mesh");
  tempStatus = rcBuildPolyMesh(&context, *contours, config.maxVertsPerPoly, *polyMesh);
  afk_assert(tempStatus, "Could not triangulate contours");

  afk_assert(polyMesh->nverts, "polymesh has no vertices");

  auto detailMesh = rcAllocPolyMeshDetail();
  afk_assert(detailMesh, "Failed to allocate poly mesh detail");
  tempStatus = rcBuildPolyMeshDetail(&context, *polyMesh, *compactHeightField,
                                     config.detailSampleDist,
                                     config.detailSampleMaxError, *detailMesh);
  afk_assert(tempStatus, "Could not build polymesh detail");

  rcFreeCompactHeightfield(compactHeightField);
  rcFreeContourSet(contours);

  // build detour nav mesh
  afk_assert(config.maxVertsPerPoly <= DT_VERTS_PER_POLYGON,
             "Too many vertices per poly");

  // update poly flags from areas
  for (int i = 0; i < polyMesh->npolys; ++i) {
    if (polyMesh->areas[i] == RC_WALKABLE_AREA) {
      polyMesh->flags[i] = POLYFLAGS_WALK;
    }
  }
  dtNavMeshCreateParams params = {};
  params.verts                 = polyMesh->verts;
  params.vertCount             = polyMesh->nverts;
  params.polys                 = polyMesh->polys;
  params.polyAreas             = polyMesh->areas;
  params.polyFlags             = polyMesh->flags;
  params.polyCount             = polyMesh->npolys;
  params.nvp                   = polyMesh->nvp;
  params.detailMeshes          = detailMesh->meshes;
  params.detailVerts           = detailMesh->verts;
  params.detailVertsCount      = detailMesh->nverts;
  params.detailTris            = detailMesh->tris;
  params.detailTriCount        = detailMesh->ntris;
  // optional
  //    params.offMeshConVerts = m_geom->getOffMeshConnectionVerts();
  //    params.offMeshConRad = m_geom->getOffMeshConnectionRads();
  //    params.offMeshConDir = m_geom->getOffMeshConnectionDirs();
  //    params.offMeshConAreas = m_geom->getOffMeshConnectionAreas();
  //    params.offMeshConFlags = m_geom->getOffMeshConnectionFlags();
  //    params.offMeshConUserID = m_geom->getOffMeshConnectionId();
  //    params.offMeshConCount = m_geom->getOffMeshConnectionCount();
  params.walkableHeight = agentHeight;
  params.walkableRadius = agentRadius;
  params.walkableClimb  = agentMaxClimb;
  rcVcopy(params.bmin, polyMesh->bmin);
  rcVcopy(params.bmax, polyMesh->bmax);
  params.cs          = config.cs;
  params.ch          = config.ch;
  params.buildBvTree = true;

  int navDataSize = 0;
  // todo: fix this, tf is it doing
  unsigned char *navData = 0;
  tempStatus             = dtCreateNavMeshData(&params, &navData, &navDataSize);
  afk_assert(tempStatus, "Failed to allocate nav mesh data");

  nav_mesh = dtAllocNavMesh();
  afk_assert(nav_mesh, "Failed to allocate nav mesh");

  dtStatus detourStatus = nav_mesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
  afk_assert(!dtStatusFailed(detourStatus), "Could not init detour navmesh");

  this->create_nav_mesh_model(*nav_mesh);

  // should be successful, if something failed afk_assert should have picked it up
  return true;
}

void NavMeshManager::get_min_max_bounds(const Afk::Mesh &mesh, glm::vec3 &min,
                                        glm::vec3 &max) {
  min.x = 0.0f;
  min.y = 0.0f;
  min.z = 0.0f;
  max.x = 0.0f;
  max.y = 0.0f;
  max.z = 0.0f;
  for (const auto &vertex : mesh.vertices) {
    if (vertex.position.x < min.x) {
      min.x = vertex.position.x;
    } else if (vertex.position.x > max.x) {
      max.x = vertex.position.x;
    }

    if (vertex.position.y < min.y) {
      min.y = vertex.position.y;
    } else if (vertex.position.y > max.y) {
      max.y = vertex.position.y;
    }

    if (vertex.position.z < min.z) {
      min.z = vertex.position.z;
    } else if (vertex.position.z > max.z) {
      max.z = vertex.position.z;
    }
  }
}

const Afk::Model &NavMeshManager::get_height_field_model() {
  return height_field_model;
}

const Afk::Model &NavMeshManager::get_nav_mesh_model() {
  return nav_mesh_model;
}

// save files may not necessarily be compatible between different systems
bool NavMeshManager::load(const std::filesystem::path &file_path) {
  bool output = false;

  std::ifstream in(file_path, std::ios::binary);
  if (in) {
    // todo: check if enough data exists
    NavMeshSetHeader header = {};
    in.read(reinterpret_cast<char *>(&header), sizeof(NavMeshSetHeader));
    if (header.magic == NAVMESHSET_MAGIC && header.version == NAVMESHSET_VERSION) {
      // fix memory leak
      nav_mesh = dtAllocNavMesh();
      if (nav_mesh) {
        dtStatus status = nav_mesh->init(&header.params);
        if (!dtStatusFailed(status)) {
          // read tiles
          for (int i = 0; i < header.numTiles; i++) {
            NavMeshTileHeader tile_header = {};
            in.read(reinterpret_cast<char *>(&tile_header), sizeof(NavMeshTileHeader));

            if (!tile_header.tileRef || !tile_header.dataSize) {
              output = false;
              break;
            }

            // todo: fix
            auto *data = (unsigned char *)dtAlloc(tile_header.dataSize, DT_ALLOC_PERM);
            if (!data) {
              output = false;
              break;
            }

            memset(data, 0, tile_header.dataSize);
            // todo: check if you can read ahead
            in.read(reinterpret_cast<char *>(data), tile_header.dataSize);
            nav_mesh->addTile(data, tile_header.dataSize, DT_TILE_FREE_DATA,
                              tile_header.tileRef, nullptr);
            output = true;
          }
          create_nav_mesh_model(*nav_mesh);
        }
      }
    }

    in.close();
  }

  return output;
}

// save files may not necessarily be compatible between different systems
bool NavMeshManager::save(const std::filesystem::path &file_path) {
  bool output = false;

  const dtNavMesh *mesh = nav_mesh;

  if (mesh) {
    std::ofstream out(file_path, std::ios::binary);
    if (out) {
      NavMeshSetHeader header = {};
      header.magic            = NAVMESHSET_MAGIC;
      header.version          = NAVMESHSET_VERSION;
      header.params   = *(mesh->getParams()); // double check if this is ok
      header.numTiles = 0;
      for (int i = 0; i < mesh->getMaxTiles(); i++) {
        const dtMeshTile *tile = mesh->getTile(i);
        if (!tile || !tile->header || !tile->dataSize) {
          continue;
        }
        header.numTiles++;
      }
      out.write(reinterpret_cast<char *>(&header), sizeof(NavMeshSetHeader));

      // store tiles
      for (int i = 0; i < mesh->getMaxTiles(); i++) {
        const dtMeshTile *tile = mesh->getTile(i);
        if (!tile || !tile->header | !tile->dataSize) {
          continue;
        }

        NavMeshTileHeader tile_header = {};
        tile_header.tileRef           = mesh->getTileRef(tile);
        tile_header.dataSize          = tile->dataSize;
        out.write(reinterpret_cast<char *>(&tile_header), sizeof(NavMeshTileHeader));
        out.write(reinterpret_cast<char *>(tile->data), tile->dataSize);
      }

      out.close();

      output = true;
    }
  }

  return output;
}

// todo: apply transforms other tran translate
glm::vec3 NavMeshManager::transform_pos(const glm::vec3 &input,
                                        const Afk::Transform &transform) {
  return glm::vec3(input.x + transform.translation.x,
                   input.y + transform.translation.y,
                   input.z + transform.translation.z);
}

void NavMeshManager::create_height_field_model(const rcHeightfield &heightField) {
  height_field_model           = {};
  height_field_model.file_path = "res/gen/heightfield/model";
  height_field_model.file_dir  = "res/gen/heightfield";
  Mesh heightFieldMesh         = {};

  for (int y = 0; y < heightField.height; y++) {
    for (int x = 0; x < heightField.width; x++) {
      const auto minx = heightField.bmin[0] + x * heightField.cs;
      const auto minz = heightField.bmin[2] + y * heightField.cs;
      const rcSpan *s = heightField.spans[x + y * heightField.width];
      while (s) {
        const auto vertexOffset = heightFieldMesh.vertices.size();
        const auto miny = heightField.bmin[1] + s->smin * heightField.ch;
        const auto maxx = minx + heightField.cs;
        const auto maxy = heightField.bmin[1] + s->smax * heightField.ch;
        const auto maxz = minz * heightField.cs;

        // duplicate vertices may be created
        Vertex vertex   = {};
        vertex.position = glm::vec3(minx, miny, minz);
        heightFieldMesh.vertices.push_back(vertex);

        vertex.position = glm::vec3(maxx, miny, minz);
        heightFieldMesh.vertices.push_back(vertex);

        vertex.position = glm::vec3(maxx, miny, maxz);
        heightFieldMesh.vertices.push_back(vertex);

        vertex.position = glm::vec3(minx, miny, maxz);
        heightFieldMesh.vertices.push_back(vertex);

        vertex.position = glm::vec3(minx, maxy, minz);
        heightFieldMesh.vertices.push_back(vertex);

        vertex.position = glm::vec3(maxx, maxy, minz);
        heightFieldMesh.vertices.push_back(vertex);

        vertex.position = glm::vec3(maxx, maxy, maxz);
        heightFieldMesh.vertices.push_back(vertex);

        vertex.position = glm::vec3(minx, maxy, maxz);
        heightFieldMesh.vertices.push_back(vertex);

        // face
        heightFieldMesh.indices.push_back(vertexOffset);
        heightFieldMesh.indices.push_back(vertexOffset + 1);
        heightFieldMesh.indices.push_back(vertexOffset + 5);
        heightFieldMesh.indices.push_back(vertexOffset + 0);
        heightFieldMesh.indices.push_back(vertexOffset + 5);
        heightFieldMesh.indices.push_back(vertexOffset + 4);

        // face
        heightFieldMesh.indices.push_back(vertexOffset + 0);
        heightFieldMesh.indices.push_back(vertexOffset + 1);
        heightFieldMesh.indices.push_back(vertexOffset + 2);
        heightFieldMesh.indices.push_back(vertexOffset + 0);
        heightFieldMesh.indices.push_back(vertexOffset + 2);
        heightFieldMesh.indices.push_back(vertexOffset + 3);

        // face
        heightFieldMesh.indices.push_back(vertexOffset + 3);
        heightFieldMesh.indices.push_back(vertexOffset + 2);
        heightFieldMesh.indices.push_back(vertexOffset + 6);
        heightFieldMesh.indices.push_back(vertexOffset + 3);
        heightFieldMesh.indices.push_back(vertexOffset + 6);
        heightFieldMesh.indices.push_back(vertexOffset + 7);

        // face
        heightFieldMesh.indices.push_back(vertexOffset + 4);
        heightFieldMesh.indices.push_back(vertexOffset + 5);
        heightFieldMesh.indices.push_back(vertexOffset + 6);
        heightFieldMesh.indices.push_back(vertexOffset + 4);
        heightFieldMesh.indices.push_back(vertexOffset + 6);
        heightFieldMesh.indices.push_back(vertexOffset + 7);

        // face
        heightFieldMesh.indices.push_back(vertexOffset + 0);
        heightFieldMesh.indices.push_back(vertexOffset + 3);
        heightFieldMesh.indices.push_back(vertexOffset + 7);
        heightFieldMesh.indices.push_back(vertexOffset + 0);
        heightFieldMesh.indices.push_back(vertexOffset + 7);
        heightFieldMesh.indices.push_back(vertexOffset + 4);

        // face
        heightFieldMesh.indices.push_back(vertexOffset + 1);
        heightFieldMesh.indices.push_back(vertexOffset + 2);
        heightFieldMesh.indices.push_back(vertexOffset + 6);
        heightFieldMesh.indices.push_back(vertexOffset + 1);
        heightFieldMesh.indices.push_back(vertexOffset + 6);
        heightFieldMesh.indices.push_back(vertexOffset + 5);

        s = s->next;
      }
    }
  }

  height_field_model.meshes.push_back(std::move(heightFieldMesh));
}

void NavMeshManager::create_nav_mesh_model(const dtNavMesh &navMesh) {
  nav_mesh_model           = {};
  nav_mesh_model.file_path = "res/gen/navmesh/model";
  nav_mesh_model.file_dir  = "res/gen/navmesh";
  Mesh mesh                = {};

  const auto SAMPLE_POLYFLAGS_DISABLED = 0x10;
  for (int i = 0; i < navMesh.getMaxTiles(); i++) {
    const dtMeshTile *tile = navMesh.getTile(i);
    if (!tile->header) {
      continue;
    }

    dtPolyRef base = navMesh.getPolyRefBase(tile);

    for (int j = 0; j < tile->header->polyCount; j++) {
      const dtPoly *p = &tile->polys[j];

      // what does this do???
      if ((p->flags & SAMPLE_POLYFLAGS_DISABLED)) {
        continue;
      }

      const auto ref = base | (dtPolyRef)j; // ???
      NavMeshManager::process_nav_mesh_model_poly(navMesh, mesh, ref);
    }
  }

  nav_mesh_model.meshes.push_back(std::move(mesh));
}

void NavMeshManager::process_nav_mesh_model_poly(const dtNavMesh &navMesh,
                                                 Afk::Mesh &mesh, dtPolyRef ref) {
  // todo: set to nullptr if it lets me
  const dtMeshTile *tile = 0;
  const dtPoly *poly     = 0;
  if (dtStatusFailed(navMesh.getTileAndPolyByRef(ref, &tile, &poly))) {
    return;
  }

  // assume type isn't DT_POLYTYPE_OFFMESH_CONNECTION
  if (poly->getType() == DT_POLYTYPE_OFFMESH_CONNECTION) {
    return;
  }

  const unsigned int ip  = (poly - tile->polys);
  const dtPolyDetail *pd = &tile->detailMeshes[ip];

  Vertex vertex = {};
  for (int i = 0; i < pd->triCount; i++) {
    const unsigned char *t = &tile->detailTris[(pd->triBase + i) * 4];

    const auto vertexOffset = mesh.vertices.size();
    // duplicate vertices may be created
    for (int j = 0; j < 3; j++) {
      float *pos = nullptr;
      if (t[j] < poly->vertCount) {
        pos = &tile->verts[poly->verts[t[j]] * 3];
      } else {
        pos = &tile->detailVerts[(pd->vertBase + t[j] - poly->vertCount) * 3];
      }
      afk_assert(pos != nullptr, "vertex not found");
      vertex.position.x = pos[0];
      vertex.position.y = pos[1];
      vertex.position.z = pos[2];
      mesh.vertices.push_back(vertex);

      mesh.indices.push_back(vertexOffset);
      mesh.indices.push_back(vertexOffset + 1);
      mesh.indices.push_back(vertexOffset + 2);
    }
  }
}

dtNavMesh *NavMeshManager::get_nav_mesh() {
  return this->nav_mesh;
}
