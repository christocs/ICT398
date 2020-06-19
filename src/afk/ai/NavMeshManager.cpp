#include "NavMeshManager.hpp"

#include <fstream>
#include <memory>

#include "afk/debug/Assert.hpp"
#include "afk/io/ModelSource.hpp"
#include <glm/gtc/type_ptr.inl>

#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h"
#include "DetourNavMeshQuery.h"
#include "afk/io/Log.hpp"
#include "afk/component/TagComponent.hpp"

using Afk::AI::NavMeshManager;

// call AFTER all static entities have been assigned
// looks at all render meshes that have a transform, model and physicsbody that
// is static
// todo: remove cout
bool NavMeshManager::initialise(const std::filesystem::path &file_path) {
  this->file_path_ = file_path;
  if (!this->load(file_path)) {
    if (this->bake()) {
      if (!this->save(file_path)) {
        Afk::Io::log << "Failed to save nav mesh" << '\n';
        return false;
      } else {
        Afk::Io::log << "Nav mesh saved" << '\n';
      }
    } else {
      Afk::Io::log << "Failed to bake nav mesh" << '\n';
      return false;
    }
  } else {
    Afk::Io::log << "Nav mesh successfully loaded" << '\n';
  }
  create_nav_mesh_model(*nav_mesh);
  return true;
}

// call AFTER all static entities have been assigned
// looks at all render meshes that have a transform, model and physicsbody that is static
bool NavMeshManager::bake() {
  auto physics_model_view =
      Afk::Engine::get().registry.view<Afk::Transform, Afk::Model, Afk::PhysicsBody, Afk::TagComponent>();

  size_t nvertices = 0;
  size_t nindices  = 0;
  for (const auto &entity : physics_model_view) {
    const auto &model_component = physics_model_view.get<Afk::Model>(entity);
    const auto &model_physics_body = physics_model_view.get<Afk::PhysicsBody>(entity);
    const auto &model_tag_component = physics_model_view.get<Afk::TagComponent>(entity);
    // make sure physics body is static
    if (model_physics_body.get_type() == Afk::RigidBodyType::STATIC) {
      // make sure the entity is tagged as terrain
      if (model_tag_component.tags.count(Afk::TagComponent::Tag::TERRAIN) == 1) {
        for (const auto &mesh : model_component.meshes) {
          nvertices += mesh.vertices.size();
          nindices += mesh.indices.size();
        }
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

        // int make sure the order of the indices is correct!
        const auto &indices = mesh.indices;
        for (size_t i = 0; i < indices.size(); i += 3) {
          triangles.push_back(static_cast<int>(indices[i] + vertex_offset));
          triangles.push_back(static_cast<int>(indices[i + 1] + vertex_offset));
          triangles.push_back(static_cast<int>(indices[i + 2] + vertex_offset));
        }

        vertex_offset += mesh.vertices.size();
        index_offset += mesh.indices.size();
      }
    }
  }

  glm::vec3 bmin = {};
  glm::vec3 bmax = {};
  NavMeshManager::get_min_max_bounds(vertices, bmin, bmax);

  auto chunky_mesh = std::make_shared<ChunkyTriMesh>();

  auto check_success = chunky_mesh.get()->init(vertices.data(), triangles.data(),
                                         ntriangles, 256, chunky_mesh.get());
  afk_assert(check_success, "Failed to create chunky triangle mesh");

  const float grid_cell_size = 0.15f; // determines resolution when voxelising nav meshes
  const int grid_width = static_cast<int>(((bmax.x - bmin.x) / grid_cell_size) + 0.5f);
  const int grid_height = static_cast<int>(((bmax.z - bmin.z) / grid_cell_size) + 0.5f);

  //  const int tile_size        = 8;
  const int tile_size        = 64;
  const int tile_width       = (grid_width + tile_size - 1) / tile_size;
  const int tile_height      = (grid_height + tile_size - 1) / tile_size;
  const float tile_cell_size = tile_size * grid_cell_size;

  dtNavMeshParams params = {};
  params.tileWidth       = tile_cell_size;
  params.tileHeight      = tile_cell_size;
  // Max tiles and max polys affect how the tile IDs are caculated.
  // There are 22 bits available for identifying a tile and a polygon.
  int tileBits = rcMin((int)ilog2(nextPow2(tile_width * tile_height)), 14);
  if (tileBits > 14) {
    tileBits = 14;
  }
  int polyBits    = 22 - tileBits;
  params.maxTiles = 1 << tileBits;
  params.maxPolys = 1 << polyBits;

  nav_mesh         = nav_mesh_ptr{dtAllocNavMesh(), &dtFreeNavMesh};
  auto temp_status = nav_mesh->init(&params);
  afk_assert(!dtStatusFailed(temp_status), "Failed to init nav mesh");

  glm::vec3 tile_bmin = {0.0f, bmin.y, 0.0f};
  glm::vec3 tile_bmax = {0.0f, bmax.y, 0.0f};
  for (int y = 0; y < tile_height; y++) {
    for (int x = 0; x < tile_width; x++) {
      tile_bmin.x = bmin.x + x * tile_cell_size;
      tile_bmin.z = bmin.z + y * tile_cell_size;
      tile_bmax.x = bmin.x + (x + 1) * tile_cell_size;
      tile_bmax.z = bmin.z + (y + 1) * tile_cell_size;
      //      std::cout << "bounds: "  << tile_bmin.x << ", " << tile_bmin.z << " - " << tile_bmax.x <<  ", " << tile_bmax.z << std::endl;
      //      continue;

      int data_size = 0;
      auto data     = this->build_tile_nav_mesh(x, y, tile_bmin, tile_bmax,
                                            grid_cell_size, tile_size, data_size,
                                            chunky_mesh, vertices, triangles);

      if (data) {
        // Remove any previous data (navmesh owns and deletes the data).
        nav_mesh->removeTile(nav_mesh->getTileRefAt(x, y, 0), 0, 0);
        // Let the navmesh own the data.
        temp_status = nav_mesh->addTile(data, data_size, DT_TILE_FREE_DATA, 0, 0);
        //        std::cout << "after add tile: " << temp_status << " " << data << std::endl;
        if (dtStatusFailed(temp_status)) {
          dtFree(data);
          //          afk_assert(!dtStatusFailed(temp_status),
          //                     "failed to add tile to nav mesh");
        }
      }
    }
  }

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

          afk_assert(tile_header.dataSize >= 0,
                     "Tile header data size cannot be negative");
          // todo: fix
          auto *data = static_cast<unsigned char *>(
              dtAlloc(static_cast<size_t>(tile_header.dataSize), DT_ALLOC_PERM));
          if (!data) {
            output = false;
            break;
          }

          memset(data, 0, static_cast<size_t>(tile_header.dataSize));
          // todo: check if you can read ahead
          in.read(reinterpret_cast<char *>(data),
                  static_cast<long long>(tile_header.dataSize));
          nav_mesh->addTile(data, tile_header.dataSize, DT_TILE_FREE_DATA,
                            tile_header.tileRef, nullptr);
          output = true;
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

void NavMeshManager::create_height_field_model(const rcHeightfield &height_field) {
  height_field_model           = {};
  height_field_model.file_path = "res/gen/heightfield/model";
  height_field_model.file_dir  = "res/gen/heightfield";
  Mesh mesh                    = {};
  mesh.transform.translation   = glm::vec3(0.0f);

  for (int y = 0; y < height_field.height; y++) {
    for (int x = 0; x < height_field.width; x++) {
      const auto minx = height_field.bmin[0] + x * height_field.cs;
      const auto minz = height_field.bmin[2] + y * height_field.cs;
      const rcSpan *s = height_field.spans[x + y * height_field.width];
      while (s) {
        const auto vertex_offset = static_cast<unsigned int>(mesh.vertices.size());
        const auto miny = height_field.bmin[1] + s->smin * height_field.ch;
        const auto maxx = minx + height_field.cs;
        const auto maxy = height_field.bmin[1] + s->smax * height_field.ch;
        const auto maxz = minz * height_field.cs;

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
        mesh.indices.push_back(vertex_offset);
        mesh.indices.push_back(vertex_offset + 1);
        mesh.indices.push_back(vertex_offset + 5);
        mesh.indices.push_back(vertex_offset + 0);
        mesh.indices.push_back(vertex_offset + 5);
        mesh.indices.push_back(vertex_offset + 4);

        // face
        mesh.indices.push_back(vertex_offset + 0);
        mesh.indices.push_back(vertex_offset + 1);
        mesh.indices.push_back(vertex_offset + 2);
        mesh.indices.push_back(vertex_offset + 0);
        mesh.indices.push_back(vertex_offset + 2);
        mesh.indices.push_back(vertex_offset + 3);

        // face
        mesh.indices.push_back(vertex_offset + 3);
        mesh.indices.push_back(vertex_offset + 2);
        mesh.indices.push_back(vertex_offset + 6);
        mesh.indices.push_back(vertex_offset + 3);
        mesh.indices.push_back(vertex_offset + 6);
        mesh.indices.push_back(vertex_offset + 7);

        // face
        mesh.indices.push_back(vertex_offset + 4);
        mesh.indices.push_back(vertex_offset + 5);
        mesh.indices.push_back(vertex_offset + 6);
        mesh.indices.push_back(vertex_offset + 4);
        mesh.indices.push_back(vertex_offset + 6);
        mesh.indices.push_back(vertex_offset + 7);

        // face
        mesh.indices.push_back(vertex_offset + 0);
        mesh.indices.push_back(vertex_offset + 3);
        mesh.indices.push_back(vertex_offset + 7);
        mesh.indices.push_back(vertex_offset + 0);
        mesh.indices.push_back(vertex_offset + 7);
        mesh.indices.push_back(vertex_offset + 4);

        // face
        mesh.indices.push_back(vertex_offset + 1);
        mesh.indices.push_back(vertex_offset + 2);
        mesh.indices.push_back(vertex_offset + 6);
        mesh.indices.push_back(vertex_offset + 1);
        mesh.indices.push_back(vertex_offset + 6);
        mesh.indices.push_back(vertex_offset + 5);

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

      const auto ref = base | static_cast<dtPolyRef>(j); // ???
      NavMeshManager::process_nav_mesh_model_poly(navMesh, mesh, ref);
    }
  }

  nav_mesh_model.meshes.push_back(std::move(mesh));
}

void NavMeshManager::process_nav_mesh_model_poly(const dtNavMesh &navMesh,
                                                 Afk::Mesh &mesh, dtPolyRef ref) {
  const dtMeshTile *tile = nullptr;
  const dtPoly *poly     = nullptr;
  if (dtStatusFailed(navMesh.getTileAndPolyByRef(ref, &tile, &poly))) {
    return;
  }

  // assume type isn't DT_POLYTYPE_OFFMESH_CONNECTION
  if (poly->getType() == DT_POLYTYPE_OFFMESH_CONNECTION) {
    return;
  }

  const size_t ip        = (poly - tile->polys);
  const dtPolyDetail *pd = &tile->detailMeshes[ip];

  Vertex vertex = {};
  for (unsigned int i = 0; i < pd->triCount; i++) {
    const unsigned char *t = &tile->detailTris[(pd->triBase + i) * 4];

    const auto vertexOffset = static_cast<unsigned int>(mesh.vertices.size());
    // duplicate vertices may be created
    for (unsigned int j = 0; j < 3; j++) {
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
unsigned char *NavMeshManager::build_tile_nav_mesh(
    const int tile_x, const int tile_y, glm::vec3 bmin, glm::vec3 bmax,
    float cell_size, int tile_size, int &data_size, const std::shared_ptr<ChunkyTriMesh>& chunky_tri_mesh,
    const std::vector<float> &vertices, const std::vector<int> &triangles) {

  dtStatus temp_status = {};

  const auto ntriangles = triangles.size() / 3;
  const auto nvertices  = vertices.size() / 3;

  // most are default values from demo
  rcConfig config           = {};
  config.cs                 = cell_size; // cell size, default 0.3
  config.ch                 = 0.1f;      // cell height resolution, default 0.2
  config.walkableSlopeAngle = 55.0f;     // default 45
  const auto agent_height   = 2.0f;      // default 2.0
  config.walkableHeight     = static_cast<int>(ceilf(agent_height / config.ch));
  const auto agent_max_climb = 0.9f;
  config.walkableClimb = static_cast<int>(floorf(agent_max_climb / config.ch));
  const auto agent_radius = 0.6f;
  config.walkableRadius   = static_cast<int>(ceilf(agent_radius / config.cs));
  const auto max_edge_length    = 12.0f;
  config.maxEdgeLen             = static_cast<int>(max_edge_length / config.cs);
  config.maxSimplificationError = 1.0f;
  config.minRegionArea          = static_cast<int>(8 * 8);
  config.mergeRegionArea        = static_cast<int>(20 * 20);
  config.maxVertsPerPoly        = 6;
  const auto detail_sample_distance  = 6;
  config.detailSampleDist            = detail_sample_distance * config.cs;
  const auto detail_sample_max_error = 1.0f;
  config.detailSampleMaxError        = config.ch * detail_sample_max_error;
  config.borderSize                  = config.walkableRadius + 3.0f; // Reserve enough padding, need to have some vertices from neighbouring tiles to connect them
  config.tileSize                    = tile_size;
  config.width                       = config.tileSize + config.borderSize * 2;
  config.height                      = config.tileSize + config.borderSize * 2;

  config.bmin[0] = bmin.x - config.borderSize * config.cs;
  config.bmin[1] = bmin.y;
  config.bmin[2] = bmin.z - config.borderSize * config.cs;
  config.bmax[0] = bmax.x + config.borderSize * config.cs;
  config.bmax[1] = bmax.y;
  config.bmax[2] = bmax.z + config.borderSize * config.cs;

  auto height_field = std::unique_ptr<rcHeightfield, decltype(&rcFreeHeightField)>{
      rcAllocHeightfield(), &rcFreeHeightField};

  rcContext context = {};
  temp_status =
      rcCreateHeightfield(&context, *height_field, config.width, config.height,
                          config.bmin, config.bmax, config.cs, config.ch);
  afk_assert(temp_status, "Could not create height field");

  // Allocate array that can hold triangle flags.
  // If you have multiple meshes you need to process, allocate
  // and array which can hold the max number of triangles you need to process.
  const auto areas = std::unique_ptr<unsigned char[]>(
      new unsigned char[chunky_tri_mesh.get()->maxTrisPerChunk]);
  memset(areas.get(), 0, chunky_tri_mesh.get()->maxTrisPerChunk * sizeof(unsigned char));

  float tbmin[2], tbmax[2];
  tbmin[0] = config.bmin[0];
  tbmin[1] = config.bmin[2];
  tbmax[0] = config.bmax[0];
  tbmax[1] = config.bmax[2];
  // TODO: dynamically determine the max number of chunks, allocate on the heap to reduce memory usage (can fail to allocate memory when it is fragmented)
  const auto chunk_ids = std::unique_ptr<int[]>(new int[1024]);
  memset(chunk_ids.get(), 0, 1024 * sizeof(int));
  const int ncid =
      chunky_tri_mesh.get()->get_chunks_overlapping_rect(tbmin, tbmax, chunk_ids.get(), 1024);
  if (!ncid) {
    return nullptr;
  }

  int tile_triangle_count = 0;

  for (int i = 0; i < ncid; ++i) {
    const auto &node = chunky_tri_mesh.get()->nodes.get()[chunk_ids[i]];
    const int *ctris = &chunky_tri_mesh.get()->tris[node.i * 3];
    const int nctris = node.n;

    tile_triangle_count += nctris;

    memset(areas.get(), 0, nctris * sizeof(unsigned char)); // is this necessary?
    rcMarkWalkableTriangles(&context, config.walkableSlopeAngle, vertices.data(),
                            nvertices, ctris, nctris, areas.get());

    auto success_check =
        rcRasterizeTriangles(&context, vertices.data(), nvertices, ctris, areas.get(),
                             nctris, *height_field.get(), config.walkableClimb);
    afk_assert(success_check,
               "failed to rasterize triangles for nav mesh tile");
  }
  //  std::cout << "height field cs " << height_field->cs << std::endl;

  rcFilterLowHangingWalkableObstacles(&context, config.walkableClimb, *height_field);
  rcFilterLedgeSpans(&context, config.walkableHeight, config.walkableClimb, *height_field);
  rcFilterWalkableLowHeightSpans(&context, config.walkableHeight, *height_field);

  //  this->create_height_field_model(*height_field);

  int span_count = 0;
  for (int y = 0; y < height_field->height; ++y) {
    for (int x = 0; x < height_field->width; ++x) {
      for (rcSpan *s = height_field->spans[x + y * height_field->width]; s; s = s->next) {
        if (s->area != RC_NULL_AREA)
          span_count++;
      }
    }
  }
  //  afk_assert(span_count, "no spans found");

  // compact version
  const auto compact_height_field =
      std::unique_ptr<rcCompactHeightfield, decltype(&rcFreeCompactHeightfield)>{
          rcAllocCompactHeightfield(), &rcFreeCompactHeightfield};

  temp_status = rcBuildCompactHeightfield(&context, config.walkableHeight,
                                          config.walkableClimb, *height_field,
                                          *compact_height_field);
  afk_assert(temp_status, "Could not build compact height field");

  // Erode the walkable area by agent radius.
  temp_status = rcErodeWalkableArea(&context, config.walkableRadius, *compact_height_field);
  afk_assert(temp_status, "Could not erode walkable area");

  // monotone
  temp_status = rcBuildRegionsMonotone(&context, *compact_height_field, config.borderSize,
                                       config.minRegionArea, config.mergeRegionArea);
  afk_assert(temp_status, "Could not build monotone regions");

  // trace and simplify region contours.
  auto contours = std::unique_ptr<rcContourSet, decltype(&rcFreeContourSet)>{
      rcAllocContourSet(), &rcFreeContourSet};

  temp_status = rcBuildContours(&context, *compact_height_field, config.maxSimplificationError,
                                config.maxEdgeLen, *contours);
  afk_assert(temp_status, "Could not build contours");
  //  afk_assert(contours->nconts, "No contours generated");

  auto poly_mesh = rcAllocPolyMesh();
  afk_assert(poly_mesh, "Could not allocate poly mesh");
  temp_status = rcBuildPolyMesh(&context, *contours, config.maxVertsPerPoly, *poly_mesh);
  afk_assert(
      temp_status,
      "Could not build polymesh (possibly could not triangulate contours)");

  //  afk_assert(poly_mesh->nverts, "polymesh has no vertices");

  auto detail_mesh = rcAllocPolyMeshDetail();
  afk_assert(detail_mesh, "Failed to allocate poly mesh detail");
  temp_status = rcBuildPolyMeshDetail(&context, *poly_mesh, *compact_height_field,
                                      config.detailSampleDist,
                                      config.detailSampleMaxError, *detail_mesh);
  afk_assert(temp_status, "Could not build polymesh detail");

  // build detour nav mesh
  afk_assert(config.maxVertsPerPoly <= DT_VERTS_PER_POLYGON,
             "Too many vertices per poly");

  // update poly flags from areas
  for (int i = 0; i < poly_mesh->npolys; ++i) {
    if (poly_mesh->areas[i] == RC_WALKABLE_AREA) {
      poly_mesh->flags[i] = POLYFLAGS_WALK;
    }
  }
  dtNavMeshCreateParams params = {};
  params.verts                 = poly_mesh->verts;
  params.vertCount             = poly_mesh->nverts;
  params.polys                 = poly_mesh->polys;
  params.polyAreas             = poly_mesh->areas;
  params.polyFlags             = poly_mesh->flags;
  params.polyCount             = poly_mesh->npolys;
  params.nvp                   = poly_mesh->nvp;
  params.detailMeshes          = detail_mesh->meshes;
  params.detailVerts           = detail_mesh->verts;
  params.detailVertsCount      = detail_mesh->nverts;
  params.detailTris            = detail_mesh->tris;
  params.detailTriCount        = detail_mesh->ntris;
  // optional
  //    params.offMeshConVerts = m_geom->getOffMeshConnectionVerts();
  //    params.offMeshConRad = m_geom->getOffMeshConnectionRads();
  //    params.offMeshConDir = m_geom->getOffMeshConnectionDirs();
  //    params.offMeshConAreas = m_geom->getOffMeshConnectionAreas();
  //    params.offMeshConFlags = m_geom->getOffMeshConnectionFlags();
  //    params.offMeshConUserID = m_geom->getOffMeshConnectionId();
  //    params.offMeshConCount = m_geom->getOffMeshConnectionCount();
  params.walkableHeight = agent_height;
  params.walkableRadius = agent_radius;
  params.walkableClimb  = agent_max_climb;
  params.tileX          = tile_x;
  params.tileY          = tile_y;
  params.tileLayer      = 0;
  rcVcopy(params.bmin, poly_mesh->bmin);
  rcVcopy(params.bmax, poly_mesh->bmax);
  params.cs          = config.cs;
  params.ch          = config.ch;
  params.buildBvTree = true;

  int nav_data_size = 0;
  // todo: fix this, tf is it doing
  unsigned char *nav_data = nullptr;
  temp_status = dtCreateNavMeshData(&params, &nav_data, &nav_data_size);
  //  afk_assert(temp_status, "Failed to allocate nav mesh data");

  data_size = nav_data_size;
  return nav_data;
}

inline unsigned int NavMeshManager::nextPow2(unsigned int v) {
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  v++;
  return v;
}

inline unsigned int NavMeshManager::ilog2(unsigned int v) {
  unsigned int r;
  unsigned int shift;
  r = (v > 0xffff) << 4;
  v >>= r;
  shift = (v > 0xff) << 3;
  v >>= shift;
  r |= shift;
  shift = (v > 0xf) << 2;
  v >>= shift;
  r |= shift;
  shift = (v > 0x3) << 1;
  v >>= shift;
  r |= shift;
  r |= (v >> 1);
  return r;
}