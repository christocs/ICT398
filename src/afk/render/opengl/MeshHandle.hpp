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
      /**
       * Struct handling hashing a ctti type id.
       */
      struct IndexHash {
        /**
         * Calculates a hash for the specified ctti type id.
         *
         * @param value The specified ctti type id.
         * @param seed The hash seed.
         * @return The calculated hash.
         */
        constexpr auto operator()(const ctti::type_id_t &value, usize seed) const -> usize {
          return seed ^ value.hash();
        }
      };

      /**
       * Encapsulates a handle to a loaded mesh.
       */
      struct MeshHandle {
        /** A collection of texture handles.  */
        using Textures = std::vector<TextureHandle>;

        /** Maps ctti type ids to an OpenGL type enum. */
        static constexpr auto GL_INDICES =
            frozen::unordered_map<ctti::type_id_t, GLenum, 6, IndexHash>(
                {{ctti::type_id<i8>(), GL_BYTE},
                 {ctti::type_id<u8>(), GL_UNSIGNED_BYTE},
                 {ctti::type_id<i16>(), GL_SHORT},
                 {ctti::type_id<u16>(), GL_UNSIGNED_SHORT},
                 {ctti::type_id<i32>(), GL_INT},
                 {ctti::type_id<u32>(), GL_UNSIGNED_INT}});

        /** The OpenGL type enum being used for indices. */
        static constexpr auto INDEX = GL_INDICES.at(ctti::type_id<Index>());

        /**
         * Represents an OpenGL buffer type.
         */
        enum class Buffer {
          Vertex = 0,
          Normal,
          Uv,
          Tangent,
          Bitangent,
          BoneIndices,
          BoneWeights,
        };

        /** The mesh vertex array object. */
        GLuint vao = {};
        /** The mesh vertex buffer object. */
        GLuint vbo = {};
        /** The mesh index buffer object. */
        GLuint ibo = {};
        /** TODO: Finish  */
        GLuint bones = {};
        /** The texture handles being used by this mesh. */
        Textures textures = {};
        /** The number of indicies in this mesh. */
        usize num_indices = {};
        /** The transformation associated with this mesh. */
        physics::Transform transform = {};
      };
    }
  }
}
