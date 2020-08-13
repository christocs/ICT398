#include "afk/ui/UiManager.hpp"

#include <filesystem>
#include <vector>

#include <imgui/examples/imgui_impl_glfw.h>
#include <imgui/examples/imgui_impl_opengl3.h>
#include <imgui/imgui.h>

#include "afk/Afk.hpp"
#include "afk/debug/Assert.hpp"
#include "afk/io/Log.hpp"
#include "afk/io/Path.hpp"
#include "afk/render/Renderer.hpp"
#include "afk/ui/Unicode.hpp"
#include "cmake/Git.hpp"
#include "cmake/Version.hpp"

using afk::Engine;
using afk::ui::UiManager;
using std::vector;
using std::filesystem::path;

using Window = afk::render::Renderer::Window;

UiManager::~UiManager() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

auto UiManager::initialize(Window _window) -> void {
  afk_assert(_window != nullptr, "Window is uninitialized");
  afk_assert(!this->is_initialized, "UI manager already initialized");
  this->ini_path = afk::io::get_absolute_path(".imgui.ini").string();
  this->window   = _window;

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  auto &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.IniFilename = this->ini_path.c_str();
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(this->window, true);
  ImGui_ImplOpenGL3_Init("#version 410");

  auto *noto_sans = io.Fonts->AddFontFromFileTTF(
      afk::io::get_absolute_path("res/font/NotoSans-Regular.ttf").string().c_str(),
      UiManager::FONT_SIZE, nullptr, ui::unicode_ranges.data());
  this->fonts["Noto Sans"] = noto_sans;

  auto *source_code_pro = io.Fonts->AddFontFromFileTTF(
      afk::io::get_absolute_path("res/font/SourceCodePro-Regular.ttf").string().c_str(),
      UiManager::FONT_SIZE, nullptr, ui::unicode_ranges.data());
  this->fonts["Source Code Pro"] = source_code_pro;

  auto &style = ImGui::GetStyle();
  style.ScaleAllSizes(this->scale);
  this->is_initialized = true;
}

auto UiManager::prepare() const -> void {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

auto UiManager::draw() -> void {
  this->draw_stats();

  if (this->show_menu) {
    this->draw_menu_bar();
  }

  this->draw_about();
  this->draw_log();
  this->draw_model_viewer();

  if (this->show_imgui) {
    ImGui::ShowDemoWindow(&this->show_imgui);
  }

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

auto UiManager::draw_about() -> void {
  if (!this->show_about) {
    return;
  }

  ImGui::Begin("About", &this->show_about);
  ImGui::Text("afk engine version %s build %.6s (%s)", ui::to_cstr(AFK_VERSION),
              ui::to_cstr(GIT_HEAD_HASH), GIT_IS_DIRTY ? "dirty" : "clean");
  ImGui::Separator();
  ImGui::Text("%s", ui::to_cstr(GIT_COMMIT_SUBJECT));
  ImGui::Text("Author: %s", ui::to_cstr(GIT_AUTHOR_NAME));
  ImGui::Text("Date: %s", ui::to_cstr(GIT_COMMIT_DATE));
  ImGui::End();
}

auto UiManager::draw_menu_bar() -> void {
  auto &afk = Engine::get();

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Tools")) {
      if (ImGui::MenuItem("Log")) {
        this->show_log = true;
      }
      if (ImGui::MenuItem("Model viewer")) {
        this->show_model_viewer = true;
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Help")) {
      if (ImGui::MenuItem("About")) {
        this->show_about = true;
      }
      if (ImGui::MenuItem("Imgui")) {
        this->show_imgui = true;
      }

      ImGui::EndMenu();
    }

    if (ImGui::MenuItem("Exit")) {
      afk.exit();
    }

    ImGui::EndMainMenuBar();
  }
}

auto UiManager::draw_stats() -> void {
  const auto offset_x = 10.0f;
  const auto offset_y = 37.0f;
  static auto corner  = 1;

  auto &io = ImGui::GetIO();

  if (corner != -1) {
    const auto window_pos =
        ImVec2{(corner & 1) ? io.DisplaySize.x - offset_x : offset_x,
               (corner & 2) ? io.DisplaySize.y - offset_y : offset_y};
    const auto window_pos_pivot =
        ImVec2{(corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f};
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
  }

  ImGui::SetNextWindowBgAlpha(0.35f);
  ImGui::SetNextWindowSize({200, 100});
  if (ImGui::Begin("Stats", &this->show_stats,
                   (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoDecoration |
                       ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
                       ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav)) {

    const auto &afk   = Engine::get();
    const auto pos    = afk.camera.get_position();
    const auto angles = afk.camera.get_angles();

    ImGui::Text("%.1f fps (%.4f ms)", static_cast<f64>(io.Framerate),
                static_cast<f64>(io.Framerate) / 1000.0);
    ImGui::Separator();
    ImGui::Text("Position {%.1f, %.1f, %.1f}", static_cast<f64>(pos.x),
                static_cast<f64>(pos.y), static_cast<f64>(pos.z));
    ImGui::Text("Angles   {%.1f, %.1f}", static_cast<f64>(angles.x),
                static_cast<f64>(angles.y));

    if (ImGui::BeginPopupContextWindow()) {
      if (ImGui::MenuItem("Custom", nullptr, corner == -1)) {
        corner = -1;
      }
      if (ImGui::MenuItem("Top left", nullptr, corner == 0)) {
        corner = 0;
      }
      if (ImGui::MenuItem("Top right", nullptr, corner == 1)) {
        corner = 1;
      }
      if (ImGui::MenuItem("Bottom left", nullptr, corner == 2)) {
        corner = 2;
      }
      if (ImGui::MenuItem("Bottom right", nullptr, corner == 3)) {
        corner = 3;
      }
      if (this->show_stats && ImGui::MenuItem("Close")) {
        this->show_stats = false;
      }
      ImGui::EndPopup();
    }
  }

  ImGui::End();
}

auto UiManager::draw_log() -> void {
  if (!this->show_log) {
    return;
  }

  ImGui::SetNextWindowSize({500, 400});
  this->log.draw("Log", &this->show_log);
}

auto UiManager::draw_model_viewer() -> void {
  if (!this->show_model_viewer) {
    return;
  }

  auto &afk          = Engine::get();
  const auto &models = afk.renderer.get_models();

  ImGui::SetNextWindowSize({700, 500});

  if (ImGui::Begin("Models", &this->show_model_viewer)) {
    static auto selected = models.begin()->first;

    ImGui::BeginChild("left pane", ImVec2(250, 0), true);
    for (const auto &[key, value] : models) {
      if (ImGui::Selectable(key.string().c_str(), selected.lexically_normal() ==
                                                      key.lexically_normal())) {
        selected = key;
      }
    }

    ImGui::EndChild();
    ImGui::SameLine();

    ImGui::BeginGroup();

    ImGui::BeginChild("item view", {0, -ImGui::GetFrameHeightWithSpacing()});

    if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None)) {
      if (ImGui::BeginTabItem("Details")) {
        const auto &model = models.at(selected);
        ImGui::TextWrapped("Total meshes: %zu\n", model.meshes.size());
        ImGui::Separator();

        auto i = 0;
        for (const auto &mesh : model.meshes) {
          ImGui::TextWrapped("Mesh %d:\n", i);
          ImGui::TextWrapped("VAO: %u\n", mesh.ibo);
          ImGui::TextWrapped("VBO: %u\n", mesh.vbo);
          ImGui::TextWrapped("IBO: %u\n", mesh.ibo);
          ImGui::TextWrapped("Indices: %zu\n", mesh.num_indices);
          ImGui::Separator();
          ++i;
        }

        ImGui::EndTabItem();
      }
      ImGui::EndTabBar();
    }
    ImGui::EndChild();
    ImGui::EndGroup();
  }
  ImGui::End();
}
