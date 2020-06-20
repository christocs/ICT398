#pragma once

#include <filesystem>
#include <vector>

#include "afk/component/BaseComponent.hpp"
#include "afk/renderer/Animation.hpp"
#include "afk/renderer/Mesh.hpp"
#include "afk/renderer/Texture.hpp"

namespace Afk {
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
