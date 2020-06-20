#pragma once

#include <filesystem>
#include <string>

#include "afk/component/BaseComponent.hpp"

namespace Afk {
  class ModelSource : public BaseComponent {
  public:
    /**
     * Model component
     */
    ModelSource(GameObject e, const std::filesystem::path &name_,
                const std::filesystem::path &shader_path);
    /**
     * Model path
     */
    std::filesystem::path name;
    /**
     * Shader path
     */
    std::filesystem::path shader_program_path;
  };
}
