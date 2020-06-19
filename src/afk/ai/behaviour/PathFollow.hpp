#pragma once

#include "afk/ai/behaviour/BaseBehaviour.hpp"
#include "afk/ai/behaviour/Path.hpp"

namespace Afk {
  namespace AI {
    /**
     * Pathing behaviour
     */
    class PathFollow : public BaseBehaviour {
    public:
      auto update(const glm::vec3 &current_position) -> glm::vec3 override;
      /**
       * \param start Path begins at the closest node to this position
       * \param accept_distance Acceptable distance to consider a node pathed to
       */
      PathFollow(const Path &target, const glm::vec3 &start, float accept_distance);
      ~PathFollow() override = default;

    private:
      Path path;
      std::size_t path_index; // index of path vec currently travelling towards
      float required_dist;
    };
  }
}
