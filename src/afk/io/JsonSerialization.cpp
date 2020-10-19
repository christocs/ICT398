#include "afk/io/JsonSerialization.hpp"

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "afk/Engine.hpp"
#include "afk/debug/Assert.hpp"
#include "afk/io/Json.hpp"

using glm::mat3x3;
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

  auto from_json(const afk::io::Json &j, mat3x3 &m) -> void {
    j.at("x1").get_to(m[0][0]);
    j.at("y1").get_to(m[0][1]);
    j.at("z1").get_to(m[0][2]);

    j.at("x2").get_to(m[1][0]);
    j.at("y2").get_to(m[1][1]);
    j.at("z2").get_to(m[1][2]);

    j.at("x3").get_to(m[2][0]);
    j.at("y3").get_to(m[2][1]);
    j.at("z3").get_to(m[2][2]);
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

        // if center of mass is provided, use it, else calculate it
        if (j.find("center_of_mass") != j.end()) {
          c.center_of_mass = j.at("center_of_mass").get<glm::vec3>();
        } else {
          // order of applying transforms is scale, then rotation, then translation
          // in this order, only the last will have an effect on the center of mass for the simple objects being, so just use the translate
          c.center_of_mass = c.transform.translation;
        }
      }

      auto from_json(const Json &j, ColliderComponent &c) -> void {
        c.colliders = j.at("Colliders").get<std::vector<ColliderComponent::Collider>>();
      }

      auto from_json(const Json &j, PhysicsComponent &c) -> void {
        // no need to parse center of mass, as it is calculated with the collider data
        c.mass         = j.at("mass").get<f32>();
        c.inverse_mass = 1 / c.mass; // calculate inverse mass for later

        // get dampening
        c.linear_dampening  = j.at("linear_dampening").get<f32>();
        c.angular_dampening = j.at("angular_dampening").get<f32>();

        // get linear velocity if defined, else set it to 0
        if (j.find("linear_velocity") != j.end()) {
          c.linear_velocity = j.at("linear_velocity").get<glm::vec3>();
        } else {
          c.linear_velocity = glm::vec3{0.0f};
        }
      }
    }
  }
}