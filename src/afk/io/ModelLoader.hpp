#pragma once

#include <filesystem>
#include <tuple>

#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "afk/renderer/Model.hpp"
#include "afk/renderer/Texture.hpp"

namespace Afk {
  class ModelLoader {
  public:
    Model model = {};

    auto load(const std::filesystem::path &file_path) -> Model;

  private:
    auto process_node(const aiScene *scene, const aiNode *node, glm::mat4 transform) -> void;
    auto process_mesh(const aiScene *scene, const aiMesh *mesh, glm::mat4 transform) -> Mesh;
    auto get_vertices(const aiMesh *mesh) -> Mesh::Vertices;
    auto get_indices(const aiMesh *mesh) -> Mesh::Indices;
    auto get_textures(const aiMaterial *material) -> Mesh::Textures;
    auto get_bones(const aiMesh *mesh, Mesh::Vertices &vertices)
        -> std::pair<Mesh::Bones, Mesh::BoneMap>;
    auto get_animations(const aiScene *scene) -> Model::Animations;
    auto get_material_textures(const aiMaterial *material, Texture::Type type)
        -> Mesh::Textures;
    auto get_texture_path(const std::filesystem::path &file_path) const
        -> std::filesystem::path;
  };
}
