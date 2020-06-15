#include "afk/terrain/TerrainManager.hpp"

#include <array>
#include <cmath>
#include <utility>
#include <vector>

#include <FastNoiseSIMD/FastNoiseSIMD.h>
#include <glm/glm.hpp>

#include "afk/debug/Assert.hpp"
#include "afk/renderer/Mesh.hpp"

using std::size_t;
using std::vector;

using glm::vec2;
using glm::vec3;

using Afk::HeightMap;
using Afk::Mesh;
using Afk::Model;
using Afk::TerrainManager;
using Afk::Texture;
using Index = Mesh::Index;

auto TerrainManager::generate_height_map(int width, int length, float roughness,
                                         float scaling) -> void {
  afk_assert(width >= 1, "Invalid width");
  afk_assert(length >= 1, "Invalid length");

  const auto w = width;
  const auto l = length;

  const auto num_vertices = static_cast<size_t>(w * l);

  auto *noise = FastNoiseSIMD::NewFastNoiseSIMD();
  noise->SetFrequency(roughness);
  auto *noise_set = noise->GetSimplexFractalSet(0, 0, 0, w, 1, l);
  auto index      = size_t{0};

  this->height_map.width = width;
  this->height_map.heights.resize(num_vertices);

  for (auto y = 0; y < l; ++y) {
    for (auto x = 0; x < w; ++x) {
      this->height_map[{x, y}] = noise_set[index] * scaling;
      ++index;
    }
  }
  FastNoiseSIMD::FreeNoiseSet(noise_set);
  delete noise;
}

auto TerrainManager::generate_flat_plane(int width, int length) -> void {
  afk_assert(width >= 1, "Invalid width");
  afk_assert(length >= 1, "Invalid length");

  const auto w = width;
  const auto l = length;

  const auto num_vertices = static_cast<size_t>(w * l);
  const auto num_indices  = static_cast<size_t>((w - 1) * (l - 1) * 6);

  this->mesh.vertices.resize(num_vertices);
  this->mesh.indices.resize(num_indices);

  auto vertexIndex = size_t{0};
  for (auto y = 0; y < l; ++y) {
    for (auto x = 0; x < w; ++x) {
      this->mesh.vertices[vertexIndex].position =
          vec3{static_cast<float>(x) - static_cast<float>(width) / 2.0f, 0.0f,
               static_cast<float>(y) - static_cast<float>(length) / 2.0f};

      // FIXME
      this->mesh.vertices[vertexIndex].uvs =
          vec2{static_cast<float>(x) / 2.0f, static_cast<float>(y) / 2.0f};

      ++vertexIndex;
    }
  }

  auto indicesIndex = size_t{0};
  for (auto y = 0; y < (l - 1); ++y) {
    for (auto x = 0; x < (w - 1); ++x) {
      auto start = y * w + x;

      this->mesh.indices[indicesIndex++] = static_cast<Mesh::Index>(start);
      this->mesh.indices[indicesIndex++] = static_cast<Mesh::Index>(start + 1);
      this->mesh.indices[indicesIndex++] = static_cast<Mesh::Index>(start + w);

      this->mesh.indices[indicesIndex++] = static_cast<Mesh::Index>(start + 1);
      this->mesh.indices[indicesIndex++] = static_cast<Mesh::Index>(start + 1 + w);
      this->mesh.indices[indicesIndex++] = static_cast<Mesh::Index>(start + w);
    }
  }
}

auto TerrainManager::generate_terrain(int width, int length, float roughness,
                                      float scaling) -> void {
  afk_assert(width >= 1, "Invalid width");
  afk_assert(length >= 1, "Invalid length");

  this->generate_flat_plane(width, length);
  this->generate_height_map(width, length, roughness, scaling);

  for (auto i = std::size_t{0}; i < this->height_map.heights.size(); ++i) {
    this->mesh.vertices[i].position.y += this->height_map.heights[i];
  }
}

auto TerrainManager::get_model() -> Model {
  auto model = Model{};
  model.meshes.push_back(this->mesh);
  model.file_path = "gen/terrain/terrain";
  model.file_dir  = "gen/terrain";

  return model;
}

auto TerrainManager::initialize() -> void {
  afk_assert(!this->is_initialized, "Terrain manager already initialized");

  this->is_initialized = true;
}
