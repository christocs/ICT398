#pragma once

#include <filesystem>
#include <tuple>

#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "afk/render/Model.hpp"
#include "afk/render/Texture.hpp"

namespace afk {
  namespace io {
    class ModelLoader {
    public:
      render::Model model = {};
      /**
       * Load a model
       */
      auto load(const std::filesystem::path &file_path) -> render::Model;

    private:
      auto process_node(const aiScene *scene, const aiNode *node, glm::mat4 transform) -> void;
      auto process_mesh(const aiScene *scene, const aiMesh *mesh, glm::mat4 transform)
          -> render::Mesh;
      auto get_vertices(const aiMesh *mesh) -> render::Mesh::Vertices;
      auto get_indices(const aiMesh *mesh) -> render::Mesh::Indices;
      auto get_textures(const aiMaterial *material) -> render::Mesh::Textures;
      auto get_bones(const aiMesh *mesh, render::Mesh::Vertices &vertices)
          -> std::pair<render::Mesh::Bones, render::Mesh::BoneMap>;
      auto get_animations(const aiScene *scene) -> render::Model::Animations;
      auto get_material_textures(const aiMaterial *material, render::Texture::Type type)
          -> render::Mesh::Textures;
      auto get_texture_path(const std::filesystem::path &file_path) const
          -> std::filesystem::path;
    };
  }
}
