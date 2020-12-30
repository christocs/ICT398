#pragma once

#include <imgui/imgui.h>

#include "afk/NumericTypes.hpp"

namespace afk {
  namespace ui {
    /**
     * Encapsulates a log window for Imgui.
     */
    class Log {
    public:
      /**
       * Constructs a new log.
       */
      Log();

      /**
       * Clears the log.
       */
      auto clear() -> void;

      /**
       * Appends the target string to this log, via a formatted string.
       *
       * @param fmt The format string.
       * @param ... The list of arguments.
       */
      IM_FMTARGS(2) auto append(const char *fmt, ...) -> void;

      /**
       *  Draws this log.
       *
       * @param title The log window title.
       * @param open Pointer to the open state boolean.
       */
      auto draw(const char *title, bool *open = nullptr) -> void;

    private:
      /** The log text buffer. */
      ImGuiTextBuffer buffer = {};
      /** The log filter. */
      ImGuiTextFilter filter = {};
      /** The log line offsets. */
      ImVector<i32> line_offsets = {};
      /** Is auto scrolling enabled? */
      bool auto_scroll = true;
    };
  }
}
