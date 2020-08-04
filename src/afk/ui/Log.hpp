#pragma once

#include <imgui/imgui.h>

#include "afk/NumericTypes.hpp"

namespace afk {
  namespace ui {
    /**
     * Imgui log viewer.
     */
    class Log {
    public:
      Log();

      auto clear() -> void;
      IM_FMTARGS(2) auto append(const char *fmt, ...) -> void;
      auto draw(const char *title, bool *open = nullptr) -> void;

    private:
      ImGuiTextBuffer buffer     = {};
      ImGuiTextFilter filter     = {};
      ImVector<i32> line_offsets = {};
      bool auto_scroll           = true;
    };
  }
}
