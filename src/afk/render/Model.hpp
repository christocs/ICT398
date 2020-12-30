#pragma once

#include <filesystem>
#include <vector>

#include "afk/render/Animation.hpp"
#include "afk/render/Mesh.hpp"
#include "afk/render/Texture.hpp"

namespace afk {
  namespace render {
    /**
     * Encapsulates a 3D model.
     */
    struct Model {
      /** A collection of meshes. */
      using Meshes = std::vector<Mesh>;
      /** A collection of animations. */
      using Animations = std::vector<Animation>;

      /** The model meshes. */
      Meshes meshes = {};
      /** The model animations. */
      Animations animations = {};

      /** The model file path. */
      std::filesystem::path file_path = {};
      /** The model file directory. */
      std::filesystem::path file_dir = {};

      Model() = default;

      /**
       * Constructs and loads a model from the specified file path.
       *
       * @param _file_path The model file path.
       */
      Model(const std::filesystem::path &_file_path);
    };
  }
}
