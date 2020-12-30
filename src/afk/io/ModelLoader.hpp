#pragma once

#include <filesystem>
#include <tuple>

#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "afk/NumericTypes.hpp"
#include "afk/render/Model.hpp"
#include "afk/render/Texture.hpp"

namespace afk {
  namespace io {
    /**
     * Handles loading a model and extracting meshes, textures and, animations.
     */
    class ModelLoader {
    public:
      /** The loaded model. */
      render::Model model = {};

      /**
       * Loads a model from the specified model file.
       *
       * @param file_path The path of the file to load.
       * @return The loaded model.
       */
      auto load(const std::filesystem::path &file_path) -> render::Model;

    private:
      /**
       * Proccesses the current assimp node recursively.
       *
       * @param scene The current assimp scene.
       * @param node The current assimp node.
       * @param transform The current transformation matrix.
       */
      auto process_node(const aiScene *scene, const aiNode *node, glm::mat4 transform) -> void;

      /**
       * Proccesses the current assimp mesh.
       *
       * @param scene The current assimp scene.
       * @param mesh The current assimp mesh.
       * @param transform The current transformation matrix.
       */
      auto process_mesh(const aiScene *scene, const aiMesh *mesh, glm::mat4 transform)
          -> render::Mesh;

      /**
       * Returns the vertices at the specified assimp mesh.
       *
       * @param mesh The current assimp mesh.
       * @return The loaded vertices.
       */
      auto get_vertices(const aiMesh *mesh) -> render::Mesh::Vertices;

      /**
       * Returns all indices at the current assimp mesh.
       *
       * @param mesh The current assimp mesh.
       * @return The loaded indices.
       */
      auto get_indices(const aiMesh *mesh) -> render::Mesh::Indices;

      /**
       * Returns the textures at the current assimp material.
       *
       * @param material The current assimp material.
       * @return The loaded textures.
       */

      auto get_textures(const aiMaterial *material) -> render::Mesh::Textures;

      /**
       * Returns the bones at the current assimp mesh.
       *
       * @param mesh The current assimp mesh.
       * @param vertices The mesh vertices.
       * @return The loaded bones.
       */
      auto get_bones(const aiMesh *mesh, render::Mesh::Vertices &vertices)
          -> std::pair<render::Mesh::Bones, render::Mesh::BoneMap>;

      /**
       * Returns the animations in the specified assimp scene.
       *
       * @param scene The assimp scene to load from.
       * @return The loaded animations.
       */
      auto get_animations(const aiScene *scene) -> render::Model::Animations;

      /**
       * Returns the material textures at the current assimp material.
       *
       * @param material The current assimp material.
       * @param type The texture type to get.
       * @return The loaded material texture.
       */
      auto get_material_textures(const aiMaterial *material, render::Texture::Type type)
          -> render::Mesh::Textures;

      /**
       * Returns the texture from the texture file name.
       *
       * @param file_path The texture file name.
       * @return The texture file path.
       */
      auto get_texture_path(const std::filesystem::path &file_path) const
          -> std::filesystem::path;
    };
  }
}
