#pragma once

#include <memory>
#include <unordered_map>

#include <imgui/imgui.h>

#include "afk/NumericTypes.hpp"
#include "afk/render/Renderer.hpp"
#include "afk/ui/Log.hpp"

// FIXME: Add texture/mesh viewer
namespace afk {
  namespace ui {
    /**
     * Manages the engine user interface.
     */
    class UiManager {
    public:
      /**
       * The default font size, in pixels.
       */
      static constexpr f32 FONT_SIZE = 19.0f;

      ~UiManager();
      UiManager()                  = default;
      UiManager(UiManager &&)      = delete;
      UiManager(const UiManager &) = delete;
      auto operator=(const UiManager &) -> UiManager & = delete;
      auto operator=(UiManager &&) -> UiManager & = delete;

      bool show_menu   = false;
      afk::ui::Log log = {};

      auto initialize(render::renderer::Window _window) -> void;
      auto open() -> void;
      auto close() -> void;
      auto draw() -> void;
      auto prepare() const -> void;

    private:
      std::string ini_path            = {};
      render::renderer::Window window = {};

      bool show_stats        = true;
      bool show_imgui        = false;
      bool show_about        = false;
      bool show_log          = false;
      bool show_model_viewer = false;
      bool is_initialized    = false;
      f32 scale              = 1.5f;

      std::unordered_map<std::string, ImFont *> fonts = {};

      auto draw_menu_bar() -> void;
      auto draw_stats() -> void;
      auto draw_about() -> void;
      auto draw_log() -> void;
      auto draw_model_viewer() -> void;
    };
  }
}
