#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

#include "afk/render/Index.hpp"

namespace afk {
  namespace render {
    struct Bone {
      std::string name = {};
      Index index      = {};
      glm::mat4 offset = {};
    };
  }
}
