#include "afk/io/ModelLoader.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <unordered_map>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <cpplocate/cpplocate.h>
#include <frozen/unordered_map.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "afk/NumericTypes.hpp"
#include "afk/debug/Assert.hpp"
#include "afk/io/Log.hpp"
#include "afk/io/Path.hpp"
#include "afk/physics/Transform.hpp"
#include "afk/render/Animation.hpp"
#include "afk/render/Mesh.hpp"
#include "afk/render/Model.hpp"
#include "afk/render/Texture.hpp"

using namespace std::string_literals;
using glm::mat4;
using glm::quat;
using glm::vec2;
using glm::vec3;
using std::pair;
using std::string;
using std::unordered_map;
using std::vector;
using std::filesystem::path;

using afk::io::ModelLoader;
using afk::physics::Transform;
using afk::render::Animation;
using afk::render::Bone;
using afk::render::Mesh;
using afk::render::Model;
using afk::render::Texture;
using afk::render::Vertex;
namespace io = afk::io;

/** The assimp importer options to use. */
constexpr unsigned ASSIMP_OPTIONS =
    aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs |
    aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace |
    aiProcess_GlobalScale | aiProcess_LimitBoneWeights;

/** Maps the assimp texture types to engine ones. */
constexpr auto assimp_texture_types =
    frozen::make_unordered_map<Texture::Type, aiTextureType>({
        {Texture::Type::Diffuse, aiTextureType_DIFFUSE},
        {Texture::Type::Specular, aiTextureType_SPECULAR},
        {Texture::Type::Normal, aiTextureType_NORMALS},
        {Texture::Type::Height, aiTextureType_HEIGHT},
    });

/**
 * Converts the specified assimp matrix to a glm matrix.
 *
 * @param m The assimp matrix to convert.
 * @return The converted glm matrix.
 */
static auto to_glm(aiMatrix4x4t<f32> m) -> mat4 {
  return mat4{m.a1, m.b1, m.c1, m.d1,  //
              m.a2, m.b2, m.c2, m.d2,  //
              m.a3, m.b3, m.c3, m.d3,  //
              m.a4, m.b4, m.c4, m.d4}; //
}

/**
 * Converts the specified assimp vector to a glm vector.
 *
 * @param v The assimp vector to convert.
 * @return The converted glm vector.
 */
static auto to_glm(aiVector3t<f32> v) -> vec3 {
  return vec3{v.x, v.y, v.z};
}

/**
 * Converts the specified assimp quaternion to a glm quaternion.
 *
 * @param q The assimp quaternion to convert.
 * @return The converted glm quaternion.
 */
static auto to_glm(aiQuaterniont<f32> q) -> quat {
  return quat{q.w, q.x, q.y, q.z};
}

auto ModelLoader::load(const path &file_path) -> Model {
  const auto abs_path = io::get_absolute_path(file_path);
  auto importer       = Assimp::Importer{};

  this->model.file_path = file_path;
  this->model.file_dir  = file_path.parent_path();

  afk_assert(std::filesystem::exists(abs_path),
             "Model "s + file_path.string() + " doesn't exist"s);

  const auto *scene = importer.ReadFile(abs_path.string(), ASSIMP_OPTIONS);

  afk_assert(scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode,
             "Model load error: "s + importer.GetErrorString());

  this->model.meshes.reserve(scene->mNumMeshes);
  this->process_node(scene, scene->mRootNode, to_glm(scene->mRootNode->mTransformation));
  this->model.animations = this->get_animations(scene);

  return std::move(this->model);
}

auto ModelLoader::process_node(const aiScene *scene, const aiNode *node,
                               glm::mat4 transform) -> void {
  // Process all meshes at this node.
  for (auto i = usize{0}; i < node->mNumMeshes; ++i) {
    const auto *mesh = scene->mMeshes[node->mMeshes[i]];

    this->model.meshes.push_back(
        this->process_mesh(scene, mesh, transform * to_glm(node->mTransformation)));
  }

  // Process all child nodes.
  for (auto i = usize{0}; i < node->mNumChildren; ++i) {
    this->process_node(scene, node->mChildren[i], transform * to_glm(node->mTransformation));
  }
}

auto ModelLoader::process_mesh(const aiScene *scene, const aiMesh *mesh, mat4 transform)
    -> Mesh {
  auto new_mesh = Mesh{};

  new_mesh.vertices = this->get_vertices(mesh);
  new_mesh.indices  = this->get_indices(mesh);
  new_mesh.textures = this->get_textures(scene->mMaterials[mesh->mMaterialIndex]);

  auto [bones, bone_map] = this->get_bones(mesh, new_mesh.vertices);
  new_mesh.bones         = std::move(bones);
  new_mesh.bone_map      = std::move(bone_map);

  new_mesh.transform = transform;

  return new_mesh;
}

auto ModelLoader::get_vertices(const aiMesh *mesh) -> Mesh::Vertices {
  auto vertices      = Mesh::Vertices{};
  const auto has_uvs = mesh->HasTextureCoords(0);

  vertices.reserve(mesh->mNumVertices);

  for (auto i = usize{0}; i < mesh->mNumVertices; ++i) {
    auto vertex = Vertex{};

    vertex.position = to_glm(mesh->mVertices[i]);
    vertex.normal   = to_glm(mesh->mNormals[i]);

    if (has_uvs) {
      vertex.uvs       = to_glm(mesh->mTextureCoords[0][i]);
      vertex.tangent   = to_glm(mesh->mTangents[i]);
      vertex.bitangent = to_glm(mesh->mBitangents[i]);
    }

    vertices.push_back(std::move(vertex));
  }

  return vertices;
}

auto ModelLoader::get_indices(const aiMesh *mesh) -> Mesh::Indices {
  auto indices = Mesh::Indices{};

  indices.reserve(mesh->mNumFaces);

  for (auto i = usize{0}; i < mesh->mNumFaces; ++i) {
    const auto face = mesh->mFaces[i];

    for (auto j = usize{0}; j < face.mNumIndices; ++j) {
      indices.push_back(static_cast<afk::render::Index>(face.mIndices[j]));
    }
  }

  return indices;
}

auto ModelLoader::get_material_textures(const aiMaterial *material, Texture::Type type)
    -> Mesh::Textures {
  auto textures = Mesh::Textures{};

  const auto texture_count = material->GetTextureCount(assimp_texture_types.at(type));

  textures.reserve(texture_count);

  for (auto i = 0u; i < texture_count; ++i) {
    auto assimp_path = aiString{};
    material->GetTexture(assimp_texture_types.at(type), i, &assimp_path);
    const auto file_path = get_texture_path(path{string{assimp_path.data}});

    auto texture      = Texture{};
    texture.type      = type;
    texture.file_path = file_path;
    textures.push_back(texture);
  }

  return textures;
}

auto ModelLoader::get_textures(const aiMaterial *material) -> Mesh::Textures {
  auto textures = Mesh::Textures{};

  const auto diffuse = this->get_material_textures(material, Texture::Type::Diffuse);
  const auto specular = this->get_material_textures(material, Texture::Type::Specular);
  const auto normal = this->get_material_textures(material, Texture::Type::Normal);
  const auto height = this->get_material_textures(material, Texture::Type::Height);

  textures.insert(textures.end(), std::make_move_iterator(diffuse.begin()),
                  std::make_move_iterator(diffuse.end()));
  textures.insert(textures.end(), std::make_move_iterator(specular.begin()),
                  std::make_move_iterator(specular.end()));
  textures.insert(textures.end(), std::make_move_iterator(normal.begin()),
                  std::make_move_iterator(normal.end()));
  textures.insert(textures.end(), std::make_move_iterator(height.begin()),
                  std::make_move_iterator(height.end()));

  return textures;
}

auto ModelLoader::get_bones(const aiMesh *mesh, Mesh::Vertices &vertices)
    -> pair<Mesh::Bones, Mesh::BoneMap> {
  auto bones    = Mesh::Bones{};
  auto bone_map = Mesh::BoneMap{};

  for (auto i = usize{0}; i < mesh->mNumBones; ++i) {
    const auto &assimp_bone = *mesh->mBones[i];

    auto name = string{assimp_bone.mName.data};

    if (bone_map.find(name) == bone_map.end()) {
      auto bone           = Bone{};
      bone.name           = name;
      bone.index          = static_cast<render::Index>(bone_map.size());
      bone.offset         = to_glm(assimp_bone.mOffsetMatrix);
      bone_map[bone.name] = bone.index;
      bones.push_back(std::move(bone));
    }

    const auto bone_index = bone_map.at(name);

    for (auto j = usize{0}; j < assimp_bone.mNumWeights; j++) {
      const auto &assimp_weight = assimp_bone.mWeights[j];
      const auto vertex_index   = static_cast<usize>(assimp_weight.mVertexId);
      const auto bone_weight    = assimp_weight.mWeight;

      vertices[vertex_index].push_back_bone(bone_index, bone_weight);
    }
  }

  return std::make_pair(bones, bone_map);
}

auto ModelLoader::get_animations(const aiScene *scene) -> Model::Animations {
  auto animations = Model::Animations{};

  for (auto i = usize{0}; i < scene->mNumAnimations; ++i) {
    const auto &assimp_animation = *scene->mAnimations[i];

    auto animation       = Animation{};
    animation.name       = string{assimp_animation.mName.data};
    animation.frame_rate = static_cast<f32>(assimp_animation.mTicksPerSecond);
    animation.duration   = static_cast<f32>(assimp_animation.mDuration);

    for (auto j = usize{0}; j < assimp_animation.mNumChannels; ++j) {
      const auto &assimp_channel = *assimp_animation.mChannels[j];

      auto channel = Animation::Channel{};
      channel.name = string{assimp_channel.mNodeName.data};

      for (auto k = usize{0}; k < assimp_channel.mNumPositionKeys; ++k) {
        auto frame        = Transform{};
        frame.translation = to_glm(assimp_channel.mPositionKeys[k].mValue);
        frame.scale       = to_glm(assimp_channel.mScalingKeys[k].mValue);
        frame.rotation    = to_glm(assimp_channel.mRotationKeys[k].mValue);

        channel.frames.push_back(frame);
      }

      animation.channels.push_back(std::move(channel));
    }

    io::log << "Loaded animation " << animation.name << " with "
            << animation.channels.size() << " channels.\n";

    animations.push_back(std::move(animation));
  }

  return animations;
}

auto ModelLoader::get_texture_path(const path &file_path) const -> path {
  return this->model.file_dir / "textures" / file_path.filename();
}
