#pragma once

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
     * Encapsulates a 3D vertex.
     */
    struct Vertex {
      /** The max number of vertex bones. */
      constexpr static usize MAX_VERTEX_BONES = 4;
      /** The max number of bones. */
      constexpr static usize MAX_BONES = 100;

      /** The position. */
      glm::vec3 position = {};
      /** The normal vector. */
      glm::vec3 normal = {};
      /** The texture positions. */
      glm::vec2 uvs = {};
      /** The tangent. */
      glm::vec3 tangent = {};
      /** The bitangent. */
      glm::vec3 bitangent = {};
      /** The bone indices. */
      Index bone_indices[MAX_VERTEX_BONES] = {};
      /** The bone indices. */
      f32 bone_weights[MAX_VERTEX_BONES] = {};

      /**
       * Pushes back a specified bone into the bone buffer.
       *
       * @param bone_index The bone index to use.
       * @param bone_weight The weight of the bone.
       */
      auto push_back_bone(Index bone_index, f32 bone_weight) -> void;
    };

    /**
     * Encapsulates a 3D mesh.
     */
    struct Mesh {
      /** A collection of vertices. */
      using Vertices = std::vector<Vertex>;
      /** A collection of indices. */
      using Indices = std::vector<Index>;
      /** A collection of textures. */
      using Textures = std::vector<Texture>;
      /** A collection of bones. */
      using Bones = std::vector<Bone>;
      /** A map of bone names to their bone index. */
      using BoneMap = std::unordered_map<std::string, Index>;

      /** The mesh vertices. */
      Vertices vertices = {};
      /** The mesh indices. */
      Indices indices = {};
      /** The mesh textures. */
      Textures textures = {};
      /** The mesh transformation. */
      physics::Transform transform = {};
      /** The mesh bones. */
      Bones bones = {};
      /** Maps bone names to their bone index. */
      BoneMap bone_map = {};
    };
  }
}
