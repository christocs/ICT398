#pragma once

#include <filesystem>
#include <vector>

#include "afk/component/BaseComponent.hpp"
#include "afk/render/Animation.hpp"
#include "afk/render/Mesh.hpp"
#include "afk/render/Texture.hpp"

namespace afk {
  namespace render {
    /**
     * Model component
     */
    struct Model : public BaseComponent {
      using Meshes     = std::vector<Mesh>;
      using Animations = std::vector<Animation>;

      Meshes meshes         = {};
      Animations animations = {};

      std::filesystem::path file_path = {};
      std::filesystem::path file_dir  = {};

      Model() = default;
      Model(GameObject e);
      Model(const std::filesystem::path &_file_path);
      Model(GameObject e, const std::filesystem::path &_file_path);
      Model(GameObject e, const Model &source);
    };
  }
}
