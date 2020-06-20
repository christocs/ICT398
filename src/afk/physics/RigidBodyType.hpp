#pragma once

namespace Afk {
  /**
   * Rigid body type enum
   * STATIC does not move
   * KINEMATIC moves but with manual changes
   * DYNAMIC moves on its own
   */
  enum class RigidBodyType { STATIC, KINEMATIC, DYNAMIC };
}
