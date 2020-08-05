#pragma once

#include <cstddef>
#include <typeindex>
#include <typeinfo>
#include <vector>

#include <ctti/type_id.hpp>
#include <frozen/unordered_map.h>
#include <glad/glad.h>

#include "afk/NumericTypes.hpp"
#include "afk/physics/Transform.hpp"
#include "afk/render/Mesh.hpp"
#include "afk/render/opengl/TextureHandle.hpp"
#include "afk/utility/ArrayOf.hpp"

namespace afk {
  namespace render {
    namespace opengl {
      struct IndexHash {
        constexpr auto operator()(const ctti::type_id_t &value, usize seed) const -> usize {
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
                {{ctti::type_id<i8>(), GL_BYTE},
                 {ctti::type_id<u8>(), GL_UNSIGNED_BYTE},
                 {ctti::type_id<i16>(), GL_SHORT},
                 {ctti::type_id<u16>(), GL_UNSIGNED_SHORT},
                 {ctti::type_id<i32>(), GL_INT},
                 {ctti::type_id<u32>(), GL_UNSIGNED_INT}});

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

        GLuint vao        = {};
        GLuint vbo        = {};
        GLuint ibo        = {};
        GLuint bones      = {};
        Textures textures = {};
        usize num_indices = {};

        physics::Transform transform = {};
      };
    }
  }
}
