#pragma once

#include <memory>
#include <unordered_map>

#include <imgui/imgui.h>

#include "afk/NumericTypes.hpp"
#include "afk/render/Renderer.hpp"
#include "afk/ui/Log.hpp"

namespace afk {
  namespace ui {
    /**
     * Manages the engine user interface.
     * @todo Add texture/mesh viewer.
     */
    class UiManager {
    public:
      /** The default font size, in pixels. */
      static constexpr f32 FONT_SIZE = 19.0f;

      ~UiManager();
      UiManager()                  = default;
      UiManager(UiManager &&)      = delete;
      UiManager(const UiManager &) = delete;
      auto operator=(const UiManager &) -> UiManager & = delete;
      auto operator=(UiManager &&) -> UiManager & = delete;

      /** Should the menu be drawn? */
      bool show_menu = false;
      /** The UI logger. */
      afk::ui::Log log = {};

      /**
       * Initializes this UI manager.
       *
       * @param _window The window to render to.
       */
      auto initialize(render::Renderer::Window _window) -> void;

      /**
       * Prepares the UI manager to render a frame.
       */
      auto prepare() const -> void;

      /**
       * Draws the UI.
       */
      auto draw() -> void;

    private:
      /** A map of font names to loaded imgui font pointers. */
      using Fonts = std::unordered_map<std::string, ImFont *>;

      /** The imgui ini path. */
      std::string ini_path = {};
      /** The window to draw to. */
      render::Renderer::Window window = {};

      /** Should the stats window be shown? */
      bool show_stats = true;
      /** Should the imgui window be shown? */
      bool show_imgui = false;
      /** Should the about window be shown? */
      bool show_about = false;
      /** Should the log window be shown? */
      bool show_log = false;
      /** Should the model viewer window be shown? */
      bool show_model_viewer = false;
      /** Is the UI manager initialized? */
      bool is_initialized = false;
      /** The UI scaling factor, where 1.0 is unscaled. */
      f32 scale = 1.5f;

      /** The loaded imgui fonts. */
      Fonts fonts = {};

      /**
       * Draws the menu bar.
       */
      auto draw_menu_bar() -> void;

      /**
       * Draws the stats overlay.
       */
      auto draw_stats() -> void;

      /**
       * Draws the about window.
       */
      auto draw_about() -> void;

      /**
       * Draws the log window.
       */
      auto draw_log() -> void;

      /**
       * Draws the model viewer window.
       */
      auto draw_model_viewer() -> void;
    };
  }
}
