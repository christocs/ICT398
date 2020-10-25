#pragma once

#include "afk/io/Json.hpp"
#include "afk/physics/Transform.hpp"
#include "afk/render/Renderer.hpp"
#include "afk/render/Model.hpp"

namespace afk {
  namespace ecs {
    namespace component {
      /**
       * Encapsulates a model component. Contains all the data needed to
       * draw an entity as a 3D model.
       */
      struct ModelComponent {
        /** The entity model. */
        afk::render::ModelHandle model_handle = {};
        /** The model transform. */
        afk::physics::Transform transform = {};
      };
    }
  }
}
