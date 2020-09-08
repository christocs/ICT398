#pragma once
#include <filesystem>

namespace afk{
  namespace render {
    struct DrawModel {
      /** The path of the model to draw. */
      const std::filesystem::path model_path = {};
      /** The shader program to use while drawing. */
      const std::filesystem::path shader_program_path = {};
    };
  }
}
