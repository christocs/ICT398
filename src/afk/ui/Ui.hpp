#pragma once

#include <memory>
#include <unordered_map>

#include <imgui/imgui.h>

#include "afk/renderer/Renderer.hpp"
#include "afk/ui/Log.hpp"

// FIXME: Add log, texture/mesh viewer
namespace Afk {
  /**
   * Defines the basic UI
   */
  class Ui {
  public:
    static constexpr auto FONT_SIZE = 19.0f;

    ~Ui();
    Ui()           = default;
    Ui(Ui &&)      = delete;
    Ui(const Ui &) = delete;
    auto operator=(const Ui &) -> Ui & = delete;
    auto operator=(Ui &&) -> Ui & = delete;

    bool show_menu = false;
    Log log        = {};

    auto initialize(Renderer::Window _window) -> void;
    auto open() -> void;
    auto close() -> void;
    auto draw() -> void;
    auto prepare() const -> void;

  private:
    std::string ini_path    = {};
    Renderer::Window window = {};

    bool show_stats        = true;
    bool show_imgui        = false;
    bool show_about        = false;
    bool show_log          = false;
    bool show_model_viewer = false;
    bool is_initialized    = false;
    float scale            = 1.5f;

    std::unordered_map<std::string, ImFont *> fonts = {};

    auto draw_menu_bar() -> void;
    auto draw_stats() -> void;
    auto draw_about() -> void;
    auto draw_log() -> void;
    auto draw_model_viewer() -> void;
  };
}
