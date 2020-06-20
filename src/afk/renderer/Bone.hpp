#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

#include "afk/renderer/Index.hpp"

namespace Afk {
  struct Bone {
    std::string name = {};
    Index index      = {};
    glm::mat4 offset = {};
  };
}
