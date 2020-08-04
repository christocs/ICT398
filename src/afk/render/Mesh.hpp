#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "afk/NumericTypes.hpp"
#include "afk/physics/Transform.hpp"
#include "afk/render/Bone.hpp"
#include "afk/render/Index.hpp"
#include "afk/render/Texture.hpp"

namespace afk {
  namespace render {
    /**
     * Vertex data
     */
    struct Vertex {
      constexpr static size_t MAX_VERTEX_BONES = 4;
      constexpr static size_t MAX_BONES        = 100;

      glm::vec3 position                   = {};
      glm::vec3 normal                     = {};
      glm::vec2 uvs                        = {};
      glm::vec3 tangent                    = {};
      glm::vec3 bitangent                  = {};
      Index bone_indices[MAX_VERTEX_BONES] = {};
      f32 bone_weights[MAX_VERTEX_BONES]   = {};

      auto push_back_bone(Index bone_index, f32 bone_weight) -> void;
    };
    /**
     * Mesh
     */
    struct Mesh {
      using Vertices = std::vector<Vertex>;
      using Indices  = std::vector<Index>;
      using Textures = std::vector<Texture>;
      using Bones    = std::vector<Bone>;
      using BoneMap  = std::unordered_map<std::string, Index>;

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
      physics::Transform transform = {};
      /**
       * The mesh bones.
       */
      Bones bones = {};
      /**
       * Mapping between bone and index.
       */
      BoneMap bone_map = {};
    };
  }
}
