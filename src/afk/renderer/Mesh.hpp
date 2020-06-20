#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "afk/physics/Transform.hpp"
#include "afk/renderer/Texture.hpp"

namespace Afk {
  /**
   * Vertex data
   */
  struct Vertex {
    glm::vec3 position  = {};
    glm::vec3 normal    = {};
    glm::vec2 uvs       = {};
    glm::vec3 tangent   = {};
    glm::vec3 bitangent = {};
  };
  /**
   * Mesh
   */
  struct Mesh {
    using Vertices = std::vector<Vertex>;
    using Index    = uint32_t;
    using Indices  = std::vector<Index>;
    using Textures = std::vector<Texture>;
    /**
     * Meshes vertices
     */
    Vertices vertices = {};
    /**
     * Mesh indices
     */
    Indices indices = {};
    /**
     * Textures used
     */
    Textures textures = {};
    /**
     * Transform of mesh
     */
    Transform transform = {};
  };
}
