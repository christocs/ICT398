#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "afk/config/Config.hpp"
#include "afk/ecs/component/Component.hpp"
#include "afk/io/Json.hpp"
#include "afk/physics/Transform.hpp"

/**
 * This file handles serialization and deserialization of JSON. If you want to
 * allow a C++ type to be serialized/deserialized, call the
 * NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE macro in the appropriate namespace of the
 * type. This will define a simple serialization and deserialization function
 * for the type where each field is used as a JSON key.
 *
 * If this isn't sufficient, or you need to do something fancy, define your own
 * to_json and from_json function. In this case I've had to manually define
 * functions for glm::quat's since the JSON specifies an euler angle, not a
 * quaternion.
 *
 * See: https://github.com/nlohmann/json#arbitrary-types-conversions
 */

namespace glm {
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(glm::vec3, x, y, z)
  auto to_json(afk::io::Json &j, const glm::quat &q) -> void;
  auto from_json(const afk::io::Json &j, glm::quat &q) -> void;
}

namespace afk {
  namespace config {
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Video, resolution_width, resolution_height,
                                       fullscreen_enabled, antialiasing_samples,
                                       antialiasing_enabled, vsync_enabled)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Config, video)
  }

  namespace physics {
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Transform, translation, scale, rotation)
  }

  namespace ecs {
    namespace component {
      NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(VelocityComponent, velocity)
      NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TransformComponent, translation, scale, rotation)
      auto from_json(const afk::io::Json &j, ModelComponent &c) -> void;
      auto from_json(const afk::io::Json &j, ColliderComponent::Collider &c) -> void;
      auto from_json(const afk::io::Json &j, ColliderComponent &c) -> void;
    }
  }
}
