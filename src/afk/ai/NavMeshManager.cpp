#include "NavMeshManager.hpp"

#include <fstream>

#include <afk/debug/Assert.hpp>
#include <afk/io/ModelSource.hpp>
#include <glm/gtc/type_ptr.inl>

#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h"
#include "DetourNavMeshQuery.h"
#include "afk/io/Log.hpp"

using Afk::AI::NavMeshManager;

// call AFTER all static entities have been assigned
// looks at all render meshes that have a transform, model and physicsbody that is static
// todo: remove cout
bool NavMeshManager::initialise(const std::filesystem::path &file_path,
                                entt::registry *registry) {
//  if (!this->load(file_path)) {
//    if (this->bake(file_path, registry)) {
//      if (!this->save(file_path)) {
//        Afk::Io::log << "Failed to bake nav mesh" << '\n';
//        return false;
//      } else {
//        Afk::Io::log << "Failed to bake nav mesh" << '\n';
//      }
//    }
//  } else {
//    Afk::Io::log << "Nav mesh successfully loaded" << '\n';
//  }
  this->bake(file_path, registry);
  return true;
}

// call AFTER all static entities have been assigned
// looks at all render meshes that have a transform, model and physicsbody that is static
bool NavMeshManager::bake(const std::filesystem::path &file_path, entt::registry *registry) {
  auto physics_model_view =
      registry->view<Afk::Transform, Afk::Model, Afk::PhysicsBody>();

  size_t nvertices = 0;
  size_t nindices  = 0;
  for (const auto &entity : physics_model_view) {
    const auto &model_component = physics_model_view.get<Afk::Model>(entity);
    const auto &model_physics_body = physics_model_view.get<Afk::PhysicsBody>(entity);
    if (model_physics_body.get_type() == Afk::RigidBodyType::STATIC) {
      for (const auto &mesh : model_component.meshes) {
        nvertices += mesh.vertices.size();
        nindices += mesh.indices.size();
      }
    }
  }
  const size_t ntriangles = nindices / 3; // should be an integer

  // vertex positions are flattened out
  std::vector<float> vertices = {};
  vertices.reserve(nvertices * 3);
  // triangles are made of 3 index positions in a row
  std::vector<int> triangles = {};
  triangles.reserve(nindices);

  size_t vertex_offset = 0;
  size_t index_offset  = 0;
  for (const auto &entity : physics_model_view) {
    const auto &model_component = physics_model_view.get<Afk::Model>(entity);
    const auto &model_transform = physics_model_view.get<Afk::Transform>(entity);
    const auto &model_physics_body = physics_model_view.get<Afk::PhysicsBody>(entity);
    if (model_physics_body.get_type() == Afk::RigidBodyType::STATIC) {
      for (const auto &mesh : model_component.meshes) {
        // add vertices for mesh
        const auto &meshVertices = mesh.vertices;
        for (const auto &meshVertex : meshVertices) {
          const auto pos = NavMeshManager::transform_pos(
              NavMeshManager::transform_pos(meshVertex.position, mesh.transform), model_transform);
          vertices.push_back(pos.x);
          vertices.push_back(pos.y);
          vertices.push_back(pos.z);
        }

        // add indices for mesh, these need ot be converted from unsigned int to int
        // make sure the order of the indices is correct!
        const auto &indices = mesh.indices;
        for (size_t i = 0; i < indices.size(); i += 3) {
          triangles.push_back(indices[i] + vertex_offset);
          triangles.push_back(indices[i + 1] + vertex_offset);
          triangles.push_back(indices[i + 2] + vertex_offset);
        }

        vertex_offset += mesh.vertices.size();
        index_offset += mesh.indices.size();
      }
    }
  }

  glm::vec3 bmin = {};
  glm::vec3 bmax = {};
  NavMeshManager::get_min_max_bounds(vertices, bmin, bmax);

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

  config.bmin[0] = bmin.x;
  config.bmin[1] = bmin.y;
  config.bmin[2] = bmin.z;
  config.bmax[0] = bmax.x;
  config.bmax[1] = bmax.y;
  config.bmax[2] = bmax.z;

  rcCalcGridSize(config.bmin, config.bmax, config.cs, &config.width, &config.height);

  auto heightField = std::unique_ptr<rcHeightfield, decltype(&rcFreeHeightField)>{
      rcAllocHeightfield(), &rcFreeHeightField};

  rcContext context = {};
  auto tempStatus =
      rcCreateHeightfield(&context, *heightField, config.width, config.height,
                          config.bmin, config.bmax, config.cs, config.ch);
  afk_assert(tempStatus, "Could not create height field");

  // Find triangles which are walkable based on their slope and rasterize them. If your input data is multiple meshes, you can transform them here, calculate the are type for each of the meshes and rasterize them.
  const auto areas = std::unique_ptr<unsigned char[]>(new unsigned char[ntriangles]);
  memset(areas.get(), 0, ntriangles * sizeof(unsigned char));

  rcMarkWalkableTriangles(&context, config.walkableSlopeAngle, vertices.data(),
                          nvertices, triangles.data(), ntriangles, areas.get());

  tempStatus = rcRasterizeTriangles(&context, vertices.data(), nvertices,
                                    triangles.data(), areas.get(), ntriangles,
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
  const auto compactHeightField =
      std::unique_ptr<rcCompactHeightfield, decltype(&rcFreeCompactHeightfield)>{
          rcAllocCompactHeightfield(), &rcFreeCompactHeightfield};

  tempStatus = rcBuildCompactHeightfield(&context, config.walkableHeight, config.walkableClimb,
                                         *heightField, *compactHeightField);
  afk_assert(tempStatus, "Could not build compact height field");

  // Erode the walkable area by agent radius.
  tempStatus = rcErodeWalkableArea(&context, config.walkableRadius, *compactHeightField);
  afk_assert(tempStatus, "Could not erode walkable area");

  // monotone
  tempStatus = rcBuildRegionsMonotone(&context, *compactHeightField, 0,
                                      config.minRegionArea, config.mergeRegionArea);
  afk_assert(tempStatus, "Could not build monotone regions");

  // trace and simplify region contours.
  auto contours = std::unique_ptr<rcContourSet, decltype(&rcFreeContourSet)>{
      rcAllocContourSet(), &rcFreeContourSet};

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

  nav_mesh = nav_mesh_ptr{dtAllocNavMesh(), &dtFreeNavMesh};

  dtStatus detourStatus = nav_mesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
  afk_assert(!dtStatusFailed(detourStatus), "Could not init detour navmesh");

  this->create_nav_mesh_model(*nav_mesh);

  // should be successful, if something failed afk_assert should have picked it up
  return true;
}

void NavMeshManager::get_min_max_bounds(const std::vector<float> &vertices,
                                        glm::vec3 &min, glm::vec3 &max) {
  if (vertices.size() < 3) {
    min.x = 0.0f;
    min.y = 0.0f;
    min.z = 0.0f;
    max.x = 0.0f;
    max.y = 0.0f;
    max.z = 0.0f;
  } else {
    min.x = vertices[0];
    min.y = vertices[1];
    min.z = vertices[2];
    max.x = vertices[0];
    max.y = vertices[1];
    max.z = vertices[2];

    for (size_t i = 3; i < vertices.size(); i += 3) {
      if (vertices[i] < min.x) {
        min.x = vertices[i];
      } else if (vertices[i] > max.x) {
        max.x = vertices[i];
      }

      if (vertices[i + 1] < min.y) {
        min.y = vertices[i + 1];
      } else if (vertices[i + 1] > max.y) {
        max.y = vertices[i + 1];
      }

      if (vertices[i + 2] < min.z) {
        min.z = vertices[i + 2];
      } else if (vertices[i + 2] > max.z) {
        max.z = vertices[i + 2];
      }
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
      nav_mesh        = nav_mesh_ptr{dtAllocNavMesh(), &dtFreeNavMesh};
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

    in.close();
  }

  return output;
}

// save files may not necessarily be compatible between different systems
bool NavMeshManager::save(const std::filesystem::path &file_path) {
  bool output = false;

  const auto *mesh = nav_mesh.get();

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

// todo: apply rotation
glm::vec3 NavMeshManager::transform_pos(const glm::vec3 &input,
                                        const Afk::Transform &transform) {
  return glm::vec3((input.x * transform.scale.x) + transform.translation.x,
                   (input.y * transform.scale.y) + transform.translation.y,
                   (input.z * transform.scale.z) + transform.translation.z);
}

void NavMeshManager::create_height_field_model(const rcHeightfield &heightField) {
  height_field_model           = {};
  height_field_model.file_path = "res/gen/heightfield/model";
  height_field_model.file_dir  = "res/gen/heightfield";
  Mesh mesh                    = {};
  mesh.transform.translation   = glm::vec3(0.0f);

  for (int y = 0; y < heightField.height; y++) {
    for (int x = 0; x < heightField.width; x++) {
      const auto minx = heightField.bmin[0] + x * heightField.cs;
      const auto minz = heightField.bmin[2] + y * heightField.cs;
      const rcSpan *s = heightField.spans[x + y * heightField.width];
      while (s) {
        const auto vertexOffset = mesh.vertices.size();
        const auto miny = heightField.bmin[1] + s->smin * heightField.ch;
        const auto maxx = minx + heightField.cs;
        const auto maxy = heightField.bmin[1] + s->smax * heightField.ch;
        const auto maxz = minz * heightField.cs;

        // duplicate vertices may be created
        Vertex vertex   = {};
        vertex.position = glm::vec3(minx, miny, minz);
        mesh.vertices.push_back(vertex);

        vertex.position = glm::vec3(maxx, miny, minz);
        mesh.vertices.push_back(vertex);

        vertex.position = glm::vec3(maxx, miny, maxz);
        mesh.vertices.push_back(vertex);

        vertex.position = glm::vec3(minx, miny, maxz);
        mesh.vertices.push_back(vertex);

        vertex.position = glm::vec3(minx, maxy, minz);
        mesh.vertices.push_back(vertex);

        vertex.position = glm::vec3(maxx, maxy, minz);
        mesh.vertices.push_back(vertex);

        vertex.position = glm::vec3(maxx, maxy, maxz);
        mesh.vertices.push_back(vertex);

        vertex.position = glm::vec3(minx, maxy, maxz);
        mesh.vertices.push_back(vertex);

        // face
        mesh.indices.push_back(vertexOffset);
        mesh.indices.push_back(vertexOffset + 1);
        mesh.indices.push_back(vertexOffset + 5);
        mesh.indices.push_back(vertexOffset + 0);
        mesh.indices.push_back(vertexOffset + 5);
        mesh.indices.push_back(vertexOffset + 4);

        // face
        mesh.indices.push_back(vertexOffset + 0);
        mesh.indices.push_back(vertexOffset + 1);
        mesh.indices.push_back(vertexOffset + 2);
        mesh.indices.push_back(vertexOffset + 0);
        mesh.indices.push_back(vertexOffset + 2);
        mesh.indices.push_back(vertexOffset + 3);

        // face
        mesh.indices.push_back(vertexOffset + 3);
        mesh.indices.push_back(vertexOffset + 2);
        mesh.indices.push_back(vertexOffset + 6);
        mesh.indices.push_back(vertexOffset + 3);
        mesh.indices.push_back(vertexOffset + 6);
        mesh.indices.push_back(vertexOffset + 7);

        // face
        mesh.indices.push_back(vertexOffset + 4);
        mesh.indices.push_back(vertexOffset + 5);
        mesh.indices.push_back(vertexOffset + 6);
        mesh.indices.push_back(vertexOffset + 4);
        mesh.indices.push_back(vertexOffset + 6);
        mesh.indices.push_back(vertexOffset + 7);

        // face
        mesh.indices.push_back(vertexOffset + 0);
        mesh.indices.push_back(vertexOffset + 3);
        mesh.indices.push_back(vertexOffset + 7);
        mesh.indices.push_back(vertexOffset + 0);
        mesh.indices.push_back(vertexOffset + 7);
        mesh.indices.push_back(vertexOffset + 4);

        // face
        mesh.indices.push_back(vertexOffset + 1);
        mesh.indices.push_back(vertexOffset + 2);
        mesh.indices.push_back(vertexOffset + 6);
        mesh.indices.push_back(vertexOffset + 1);
        mesh.indices.push_back(vertexOffset + 6);
        mesh.indices.push_back(vertexOffset + 5);

        s = s->next;
      }
    }
  }

  height_field_model.meshes.push_back(std::move(mesh));
}

void NavMeshManager::create_nav_mesh_model(const dtNavMesh &navMesh) {
  nav_mesh_model             = {};
  nav_mesh_model.file_path   = "res/gen/navmesh/model";
  nav_mesh_model.file_dir    = "res/gen/navmesh";
  Mesh mesh                  = {};
  mesh.transform.translation = glm::vec3(0.0f);

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

auto NavMeshManager::get_nav_mesh() -> NavMeshManager::nav_mesh_ptr {
  return this->nav_mesh;
}
