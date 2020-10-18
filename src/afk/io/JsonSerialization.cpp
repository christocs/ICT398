#include "afk/io/JsonSerialization.hpp"

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "afk/Engine.hpp"
#include "afk/debug/Assert.hpp"
#include "afk/io/Json.hpp"

using glm::quat;
using glm::vec3;

using afk::io::Json;

namespace glm {
  auto from_json(const afk::io::Json &j, quat &q) -> void {
    auto euler_angles = vec3{};

    j.at("x").get_to(euler_angles.x);
    j.at("y").get_to(euler_angles.y);
    j.at("z").get_to(euler_angles.z);

    afk_assert(euler_angles.x >= 0.0f && euler_angles.x <= 360.0f,
               "Angle x out of bounds");
    afk_assert(euler_angles.y >= 0.0f && euler_angles.y <= 360.0f,
               "Angle y out of bounds");
    afk_assert(euler_angles.z >= 0.0f && euler_angles.z <= 360.0f,
               "Angle z out of bounds");

    q = quat{euler_angles};
  }

  auto to_json(afk::io::Json &j, const quat &q) -> void {
    auto euler_angles = glm::eulerAngles(q);
    j = Json{{"x", euler_angles.x}, {"y", euler_angles.y}, {"z", euler_angles.z}};
  }
}

namespace afk {
  namespace ecs {
    namespace component {
      auto from_json(const Json &j, ModelComponent &c) -> void {}

      auto from_json(const Json &j, ColliderComponent::Collider &c) -> void {
        c.transform = j.at("Transform").get<TransformComponent>();

        const auto json_shape = j.at("Shape");
        const auto shape_type = json_shape.at("type").get<std::string>();

        if (shape_type == "sphere") {
          c.shape = physics::shape::Sphere{json_shape.at("radius").get<float>()};
        } else if (shape_type == "box") {
          c.shape = physics::shape::Box{json_shape.at("x").get<float>(),
                                        json_shape.at("y").get<float>(),
                                        json_shape.at("z").get<float>()};
        } else if (shape_type == "capsule") {
          c.shape = physics::shape::Capsule{json_shape.at("radius").get<float>(),
                                            json_shape.at("height").get<float>()};
        } else {
          afk_assert(false, "Invalid shape type " + shape_type + " provided");
        }
      }

      auto from_json(const Json &j, ColliderComponent &c) -> void {
        c.transform = j.at("Transform").get<TransformComponent>();
        c.colliders = j.at("Colliders").get<std::vector<ColliderComponent::Collider>>();
      }
    }
  }
}
