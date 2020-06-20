#pragma once

#include <filesystem>
#include <vector>

#include "afk/component/BaseComponent.hpp"
#include "afk/renderer/Mesh.hpp"
#include "afk/renderer/Texture.hpp"

namespace Afk {
  /**
   * Model component
   */
  struct Model : public BaseComponent {
    using Meshes = std::vector<Mesh>;

    Meshes meshes = {};

    std::filesystem::path file_path = {};
    std::filesystem::path file_dir  = {};

    Model() = default;
    Model(GameObject e);
    Model(const std::filesystem::path &_file_path);
    Model(GameObject e, const std::filesystem::path &_file_path);
    Model(GameObject e, const Model &source);
  };
}
