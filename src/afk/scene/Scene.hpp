#pragma once

#include "afk/prefab/Prefab.hpp"

namespace afk {
  namespace scene {
    /**
     * Encapsulates a scene.
     */
    struct Scene {
      /** A container of prefabs. */
      using Prefabs = std::vector<afk::prefab::Prefab>;

      /** The scene name. */
      std::string name = {};
      /** The scene prefabs. */
      Prefabs prefabs = {};
    };
  }
}
