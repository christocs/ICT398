#include "afk/Afk.hpp"

#include <memory>
#include <string>
#include <utility>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h"
#include "DetourNavMeshQuery.h"
#include "Recast.h"
#include "afk/asset/AssetFactory.hpp"
#include "afk/component/GameObject.hpp"
#include "afk/component/ScriptsComponent.hpp"
#include "afk/debug/Assert.hpp"
#include "afk/io/Log.hpp"
#include "afk/io/ModelSource.hpp"
#include "afk/physics/PhysicsBody.hpp"
#include "afk/physics/RigidBodyType.hpp"
#include "afk/physics/shape/Box.hpp"
#include "afk/physics/shape/Sphere.hpp"
#include "afk/renderer/ModelRenderSystem.hpp"
#include "afk/script/Bindings.hpp"
#include "afk/script/LuaInclude.hpp"

using namespace std::string_literals;

using glm::vec3;
using glm::vec4;

using Afk::Engine;
using Afk::Event;
using Afk::Texture;
using Action   = Afk::Event::Action;
using Movement = Afk::Camera::Movement;

auto Engine::initialize() -> void {
  afk_assert(!this->is_initialized, "Engine already initialized");

  this->renderer.initialize();
  this->event_manager.initialize(this->renderer.window);
  //  this->renderer.set_wireframe(true);

  this->ui.initialize(this->renderer.window);
  this->lua = luaL_newstate();
  luaL_openlibs(this->lua);
  Afk::add_engine_bindings(this->lua);

  this->terrain_manager.initialize();
  const int terrain_width  = 1024;
  const int terrain_length = 1024;
    this->terrain_manager.generate_terrain(terrain_width, terrain_length, 0.05f, 7.5f);
//  this->terrain_manager.generate_flat_plane(terrain_width, terrain_length);
  this->renderer.load_model(this->terrain_manager.get_model());

  auto terrain_entity           = registry.create();
  auto terrain_transform        = Transform{terrain_entity};
//  terrain_transform.translation = glm::vec3{0.0f, -10.0f, 0.0f};
  registry.assign<Afk::ModelSource>(terrain_entity, terrain_entity,
                                    terrain_manager.get_model().file_path,
                                    "shader/terrain.prog");
  registry.assign<Afk::Transform>(terrain_entity, terrain_entity);
  //  registry.assign<Afk::PhysicsBody>(terrain_entity, terrain_entity, &this->physics_body_system,
  //                                    terrain_transform, 0.3f, 0.0f, 0.0f, 0.0f,
  //                                    true, Afk::RigidBodyType::STATIC,
  //                                    this->terrain_manager.height_map);

  Afk::Asset::game_asset_factory("asset/basketball.lua");

  auto cam = registry.create();
  registry.assign<Afk::ScriptsComponent>(cam, cam)
      .add_script("script/component/camera_keyboard_control.lua", this->lua, &this->event_manager)
      .add_script("script/component/camera_mouse_control.lua", this->lua, &this->event_manager)
      .add_script("script/component/debug.lua", this->lua, &this->event_manager);

  //     todo: get the actual values from the mesh + transform
  const float navMeshBoundsMin[3] = {-512.0f, 0.0f, -512.0f};
  const float navMeshBoundsMax[3] = {512.0f, 0.0f, 512.0f};

  const size_t nvertices = this->terrain_manager.get_model().meshes[0].vertices.size();

  const std::unique_ptr<float[]> vertices(new float[nvertices * 3]);
  const auto &meshVertices = this->terrain_manager.mesh.vertices;
  size_t vertexCount       = 0;
  for (const auto &meshVertex : meshVertices) {
//    vertices[vertexCount++] = meshVertex.position.x;
//    vertices[vertexCount++] = meshVertex.position.y;
//    vertices[vertexCount++] = meshVertex.position.z;
    vertices[vertexCount++] = meshVertex.position.z;
    vertices[vertexCount++] = meshVertex.position.y;
    vertices[vertexCount++] = meshVertex.position.x;
  }
  //  std::cout << "hi " << vertices[5] << std::endl;
  //
  //
  const size_t ntriangles = this->terrain_manager.mesh.indices.size() / 3; // should be an integer

  // copy indicies into array of ints
  const auto &indicies = this->terrain_manager.mesh.indices;
  const std::unique_ptr<int[]> triangles(
      new int[this->terrain_manager.mesh.indices.size()]);
  std::copy(indicies.data(), indicies.data() + indicies.size(), triangles.get());

  // use default values from demo
  rcConfig config                 = {};
  config.cs                       = 0.3f;  // cell size
  config.ch                       = 0.2f;  // cell height
  config.walkableSlopeAngle       = 80.0f; // 45
  const auto agentHeight          = 2.0f;
  config.walkableHeight           = (int)ceilf(agentHeight / config.ch);
  const auto agentMaxClimb        = 5.0f; // 0.9
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

  rcVcopy(config.bmin, navMeshBoundsMin);
  rcVcopy(config.bmax, navMeshBoundsMax);
  rcCalcGridSize(config.bmin, config.bmax, config.cs, &config.width, &config.height);

  const auto heightField = rcAllocHeightfield();
  if (!heightField) {
    std::cout << "could not allocate height field" << std::endl;
  }

  rcContext context = {};
  if (!rcCreateHeightfield(&context, *heightField, config.width, config.height,
                           config.bmin, config.bmax, config.cs, config.ch)) {
    std::cout << "could not create height field" << std::endl;
  }

  const int w = heightField->width;
  const int h = heightField->height;
//  for (int y = 0; y < h; ++y) {
//    for (int x = 0; x < w; ++x) {
//      for (rcSpan *s = heightField->spans[x + y * w]; s; s = s->next) {
//        std::cout << static_cast<int>(s->area) << std::endl;
//      }
//    }
//  }

  // Find triangles which are walkable based on their slope and rasterize them. If your input data is multiple meshes, you can transform them here, calculate the are type for each of the meshes and rasterize them.
  const std::unique_ptr<unsigned char[]> areas(new unsigned char[ntriangles]);
  memset(areas.get(), 0, ntriangles * sizeof(unsigned char));
  std::cout << "area[5]: " << static_cast<int>(areas[5]) << std::endl;
  // std::cout << "vertices" << std::endl;
  //  for (int i = 0; i < nvertices; i ++) {
  //    std::cout << vertices[i] << std::endl;
  //  }
  //  std::cout << "triangles" << std::endl;
  //  for (int i = 0; i < this->terrain_manager.mesh.indices.size(); i++) {
  //    std::cout << triangles[i] << std::endl;
  //  }
  //  std::cout << "areas" << std::endl;
  //  for (int i = 0; i < ntriangles; i++) {
  //    std::cout << areas[i] << std::endl;
  //  }
  rcMarkWalkableTriangles(&context, config.walkableSlopeAngle, vertices.get(),
                          nvertices, triangles.get(), ntriangles, areas.get());
//  for (int i = 0; i < ntriangles; ++i) {
//    std::cout << static_cast<int>(areas[i]) << std::endl;
//  }
  if (!rcRasterizeTriangles(&context, vertices.get(), nvertices, triangles.get(),
                            areas.get(), ntriangles, *heightField, config.walkableClimb)) {
    std::cout << "could not rasterize triangles " << std::endl;
  }

  std::cout << "spans: " << heightField->spans << std::endl;
  //  for (int y = 0; y < heightField->height; ++y) {
  //    for (int x = 0; x < heightField->width; ++x) {
  //      const auto& span = heightField->spans[x][y];
  ////      const rcCompactCell& c = heightField->cells[x+y*compactHeightField->width];
  //      std::cout << span.area << " " << span.smin << " " << span.smax << std::endl;
  //    }
  //  }

  //  const int w = heightField->width;
  //  const int h = heightField->height;
  //  int spanCount = 0;
  //  for (int y = 0; y < h; ++y)
  //  {
  //    for (int x = 0; x < w; ++x)
  //    {
  //      for (rcSpan* s = heightField->spans[x + y*w]; s; s = s->next)
  //      {
  //        std::cout << s->area << std::endl;
  //        if (s->area != RC_NULL_AREA)
  //          spanCount++;
  //      }
  //    }
  //  }

  rcFilterLowHangingWalkableObstacles(&context, config.walkableClimb, *heightField);
  rcFilterLedgeSpans(&context, config.walkableHeight, config.walkableClimb, *heightField);
  rcFilterWalkableLowHeightSpans(&context, config.walkableHeight, *heightField);

  int spanCount = 0;
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      for (rcSpan *s = heightField->spans[x + y * w]; s; s = s->next) {
//        std::cout << s->area << std::endl;
        if (s->area != RC_NULL_AREA)
          spanCount++;
      }
    }
  }
  afk_assert(spanCount, "no spans found");

  // compact version
  auto compactHeightField = rcAllocCompactHeightfield();
  if (!compactHeightField) {
    std::cout << "could not allocate compact height field" << std::endl;
  }
  if (!rcBuildCompactHeightfield(&context, config.walkableHeight, config.walkableClimb,
                                 *heightField, *compactHeightField)) {
    std::cout << "could not build compact height field" << std::endl;
  }

  //  for (int y = 0; y < compactHeightField->height; ++y) {
  //    for (int x = 0; x < compactHeightField->width; ++x) {
  //      const rcCompactCell& c = compactHeightField->cells[x+y*compactHeightField->width];
  //      std::cout << c.index << " " << c.count << std::endl;
  //    }
  //  }

  rcFreeHeightField(heightField);

  // Erode the walkable area by agent radius.
  if (!rcErodeWalkableArea(&context, config.walkableRadius, *compactHeightField)) {
    std::cout << "could not erode" << std::endl;
  }

  // monotone
  if (!rcBuildRegionsMonotone(&context, *compactHeightField, 0,
                              config.minRegionArea, config.mergeRegionArea)) {
    std::cout << "could not build monotone regions" << std::endl;
  }

  //
  // Step 5. Trace and simplify region contours.
  //
  auto contours = rcAllocContourSet();
  if (!rcBuildContours(&context, *compactHeightField, config.maxSimplificationError,
                       config.maxEdgeLen, *contours)) {
    std::cout << "could not build contours" << std::endl;
  }

  afk_assert(contours->nconts, "no contours");

  auto polyMesh = rcAllocPolyMesh();
  if (!rcBuildPolyMesh(&context, *contours, config.maxVertsPerPoly, *polyMesh)) {
    std::cout << "could not triangulate contours" << std::endl;
  }

  std::cout << polyMesh->nverts << " " << polyMesh->verts << std::endl;
  afk_assert(polyMesh->nverts, "polymesh no verts");
  if (!polyMesh->nverts) {
    std::cout << "polymesh no verts" << std::endl;
  }

  auto detailMesh = rcAllocPolyMeshDetail();
  if (!rcBuildPolyMeshDetail(&context, *polyMesh, *compactHeightField, config.detailSampleDist,
                             config.detailSampleMaxError, *detailMesh)) {
    std::cout << "could not build detail mesh" << std::endl;
  }

  rcFreeCompactHeightfield(compactHeightField);
  rcFreeContourSet(contours);

  // build detour nav mesh
  if (config.maxVertsPerPoly <= DT_VERTS_PER_POLYGON) {
    dtNavMeshCreateParams params = {};
    params.verts                 = polyMesh->verts;
    params.vertCount             = polyMesh->nverts;
    params.polys                 = polyMesh->polys;
    std::cout << "vertCount " << params.vertCount << std::endl
              << "polys " << params.polys << std::endl;
    params.polyAreas        = polyMesh->areas;
    params.polyFlags        = polyMesh->flags;
    params.polyCount        = polyMesh->npolys;
    params.nvp              = polyMesh->nvp;
    params.detailMeshes     = detailMesh->meshes;
    params.detailVerts      = detailMesh->verts;
    params.detailVertsCount = detailMesh->nverts;
    params.detailTris       = detailMesh->tris;
    params.detailTriCount   = detailMesh->ntris;
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
    if (!dtCreateNavMeshData(&params, &navData, &navDataSize)) {
      std::cout << "allocate fail" << std::endl;
    }
    std::cout << navData << std::endl;

    //    auto navMesh = dtAllocNavMesh();
    //    dtStatus status = navMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
    //    auto navMeshQuery = dtAllocNavMeshQuery();
    //    status = navMeshQuery->init(navMesh, 2048);
  } else {
    std::cout << "too many polygons" << std::endl;
  }/**/

  this->is_initialized = true;
}

auto Engine::get() -> Engine & {
  static auto instance = Engine{};

  return instance;
}

auto Engine::exit() -> void {
  this->is_running = false;
}

auto Engine::render() -> void {
  Afk::queue_models(&this->registry, &this->renderer);

  this->renderer.clear_screen({135.0f, 206.0f, 235.0f, 1.0f});
  this->ui.prepare();
  this->renderer.draw();
  this->ui.draw();
  this->renderer.swap_buffers();
}

auto Engine::update() -> void {
  this->event_manager.pump_events();

  if (glfwWindowShouldClose(this->renderer.window)) {
    this->is_running = false;
  }

  if (this->ui.show_menu) {
    glfwSetInputMode(this->renderer.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  } else {
    glfwSetInputMode(this->renderer.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }

  // this->update_camera();

  this->physics_body_system.update(&this->registry, this->get_delta_time());

  ++this->frame_count;
  this->last_update = Afk::Engine::get_time();
}

auto Engine::get_time() -> float {
  return static_cast<float>(glfwGetTime());
}

auto Engine::get_delta_time() -> float {
  return this->get_time() - this->last_update;
}

auto Engine::get_is_running() const -> bool {
  return this->is_running;
}
