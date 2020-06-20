#pragma once

#include <imgui/imgui.h>

namespace Afk {
  /**
   * UI based logging
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
    ImVector<int> line_offsets = {};
    bool auto_scroll           = true;
  };
}
