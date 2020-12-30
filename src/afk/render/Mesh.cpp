#include "afk/render/Mesh.hpp"

#include "afk/NumericTypes.hpp"
#include "afk/debug/Assert.hpp"

using Vertex = afk::render::Mesh::Vertex;

auto Vertex::push_back_bone(Index bone_index, f32 bone_weight) -> void {
  auto found_empty_element = false;

  for (auto i = usize{0}; i < Vertex::MAX_VERTEX_BONES; ++i) {
    if (this->bone_weights[i] == 0.0f) {
      this->bone_indices[i] = bone_index;
      this->bone_weights[i] = bone_weight;
      found_empty_element   = true;
      break;
    }
  }

  afk_assert(found_empty_element, "Vertex bones full");
}
