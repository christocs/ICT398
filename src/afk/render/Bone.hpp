#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

#include "afk/render/Index.hpp"

namespace afk {
  namespace render {
    /**
     * Encapsulates a skeletal bone used for animation.
     */
    struct Bone {
      /** The bone name. */
      std::string name = {};
      /** The bone index. */
      Index index = {};
      /** The bone offset. */
      glm::mat4 offset = {};
    };
  }
}
