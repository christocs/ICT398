#pragma once

#include <vector>

#include "afk/io/Json.hpp"
#include "afk/physics/Transform.hpp"
#include "afk/render/Model.hpp"
#include "afk/render/Renderer.hpp"

namespace afk {
  namespace ecs {
    namespace component {
      /**
       * Represents a single model to draw for the entity
       */
      struct Model {
        /** The entity model. */
        afk::render::ModelHandle model_handle = {};
        /** The model transform. */
        afk::physics::Transform transform = {};
      };

      using Models = std::vector<Model>;

      /**
       * Encapsulates a models component. Contains all the data needed to draw multiple models for an entity.
       */
      struct ModelsComponent {
        Models models = {};
      };
    }
  }
}
