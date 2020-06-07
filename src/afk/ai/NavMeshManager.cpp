#include "NavMeshManager.hpp"

#include <afk/debug/Assert.hpp>
#include <glm/gtc/type_ptr.inl>

#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h"
#include "DetourNavMeshQuery.h"
#include "afk/io/Log.hpp"

using Afk::Ai::AgentConfig;
using Afk::Ai::NavMeshManager;

bool NavMeshManager::initialise(const std::filesystem::path &file_path,
                                        const Afk::Mesh &mesh,
                                        const Afk::Transform &transform) {
  if (!this->load(file_path)) {
    if (this->bake(file_path, mesh, transform)) {
      if (!this->save(file_path)) {
        Afk::Io::log << "Failed to bake nav mesh" << '\n';
        return false;
      }
    }
  }
  return true;
}

bool NavMeshManager::bake(const std::filesystem::path &file_path,
                                   const Afk::Mesh &mesh, const Afk::Transform &transform) {
  // use default values from demo
  rcConfig config                 = {};
  config.cs                       = 0.3f; // cell size
  config.ch                       = 0.2f; // cell height
  config.walkableSlopeAngle       = 45.0f;
  const auto agentHeight          = 2.0f;
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
  this->get_min_max_bounds(mesh, transform, bmin, bmax);
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
  auto tempStatus   = false;
  tempStatus = rcCreateHeightfield(&context, *heightField, config.width, config.height,
                                   config.bmin, config.bmax, config.cs, config.ch);
  afk_assert(tempStatus, "Could not create height field");

  const size_t nvertices = mesh.vertices.size();

  const std::unique_ptr<float[]> vertices(new float[nvertices * 3]);
  const auto &meshVertices = mesh.vertices;
  size_t vertexCount       = 0;
  for (const auto &meshVertex : meshVertices) {
    vertices[vertexCount++] = meshVertex.position.z;
    vertices[vertexCount++] = meshVertex.position.y;
    vertices[vertexCount++] = meshVertex.position.x;
  }

  const size_t ntriangles = mesh.indices.size() / 3; // should be an integer

  // copy indicies into array of ints (afk stores them as unsigned int)
  const auto &indicies = mesh.indices;
  const std::unique_ptr<int[]> triangles(new int[mesh.indices.size()]);
  std::copy(indicies.data(), indicies.data() + indicies.size(), triangles.get());

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

  this->create_height_field_model(*heightField);

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

  navMesh = dtAllocNavMesh();
  afk_assert(navMesh, "Failed to allocate nav mesh");

  dtStatus detourStatus = navMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
  afk_assert(!dtStatusFailed(detourStatus), "Could not init detour navmesh");

  // should be successful, if something failed afk_assert should have picked it up
  return true;
}

void NavMeshManager::get_min_max_bounds(const Afk::Mesh &mesh, const Afk::Transform &transform,
                                        glm::vec3 &min, glm::vec3 &max) {
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
  min = NavMeshManager::transform_pos(min, transform);
  max = NavMeshManager::transform_pos(max, transform);
}

const Afk::Model &NavMeshManager::get_height_field_model() {
  return heightFieldModel;
}

// todo: implement
bool NavMeshManager::load(const std::filesystem::path &file_path) {
  return false;
}

// todo: implement
bool NavMeshManager::save(const std::filesystem::path &file_path) {
  return false;
}

// todo: apply transforms other tran translate
glm::vec3 NavMeshManager::transform_pos(const glm::vec3 &input,
                                        const Afk::Transform &transform) {
  return glm::vec3(input.x + transform.translation.x,
                   input.y + transform.translation.y,
                   input.z + transform.translation.z);
}

void NavMeshManager::create_height_field_model(const rcHeightfield &heightField) {
  heightFieldModel.file_path = "recast/height_field";
  heightFieldModel.file_dir  = "recast";
  Mesh heightFieldMesh       = {};

  for (int y = 0; y < heightField.height; y++) {
    for (int x = 0; x < heightField.width; x++) {
      float minx      = heightField.bmin[0] + x * heightField.cs;
      float minz      = heightField.bmin[2] + y * heightField.cs;
      const rcSpan *s = heightField.spans[x + y * heightField.width];
      while (s) {
        const auto vertexOffset = heightFieldMesh.vertices.size();
        const auto miny = heightField.bmin[1] + s->smin * heightField.ch;
        const auto maxx = minx + heightField.cs;
        const auto maxy = heightField.bmin[1] + s->smax * heightField.ch;
        const auto maxz = minz * heightField.cs;

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

  heightFieldModel.meshes.push_back(std::move(heightFieldMesh));
}

dtNavMesh *NavMeshManager::get_nav_mesh() {
  return this->navMesh;
}
