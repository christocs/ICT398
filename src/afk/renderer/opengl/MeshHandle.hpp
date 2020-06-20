#pragma once

#include <cstddef>
#include <cstdint>
#include <typeindex>
#include <typeinfo>
#include <vector>

#include <ctti/type_id.hpp>
#include <frozen/unordered_map.h>
#include <glad/glad.h>

#include "afk/physics/Transform.hpp"
#include "afk/renderer/Mesh.hpp"
#include "afk/renderer/opengl/TextureHandle.hpp"
#include "afk/utility/ArrayOf.hpp"

namespace Afk {
  namespace OpenGl {
    struct IndexHash {
      constexpr auto operator()(const ctti::type_id_t &value, std::size_t seed) const
          -> std::size_t {
        return seed ^ value.hash();
      }
    };
    /**
     * Mesh handle - represents a loaded mesh
     */
    struct MeshHandle {
      using Textures = std::vector<TextureHandle>;

      static constexpr auto GL_INDICES =
          frozen::unordered_map<ctti::type_id_t, GLenum, 6, IndexHash>(
              {{ctti::type_id<int8_t>(), GL_BYTE},
               {ctti::type_id<uint8_t>(), GL_UNSIGNED_BYTE},
               {ctti::type_id<int16_t>(), GL_SHORT},
               {ctti::type_id<uint16_t>(), GL_UNSIGNED_SHORT},
               {ctti::type_id<int32_t>(), GL_INT},
               {ctti::type_id<uint32_t>(), GL_UNSIGNED_INT}});

      static constexpr auto INDEX = GL_INDICES.at(ctti::type_id<Index>());

      enum class Buffer {
        Vertex = 0,
        Normal,
        Uv,
        Tangent,
        Bitangent,
        BoneIndices,
        BoneWeights
      };

      GLuint vao              = {};
      GLuint vbo              = {};
      GLuint ibo              = {};
      GLuint bones            = {};
      Textures textures       = {};
      std::size_t num_indices = {};

      Transform transform = {};
    };
  }
}
