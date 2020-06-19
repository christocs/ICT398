#include "afk/ui/Ui.hpp"

#include <filesystem>
#include <vector>

#include <imgui/examples/imgui_impl_glfw.h>
#include <imgui/examples/imgui_impl_opengl3.h>
#include <imgui/imgui.h>

#include "afk/Afk.hpp"
#include "afk/debug/Assert.hpp"
#include "afk/io/Log.hpp"
#include "afk/io/Path.hpp"
#include "afk/renderer/Renderer.hpp"
#include "afk/ai/DifficultyManager.hpp"
#include "afk/ui/Unicode.hpp"
#include "cmake/Git.hpp"
#include "cmake/Version.hpp"

using Afk::Engine;
using Afk::Ui;
using std::vector;
using std::filesystem::path;

Ui::~Ui() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

auto Ui::initialize(Renderer::Window _window) -> void {
  afk_assert(_window != nullptr, "Window is uninitialized");
  afk_assert(!this->is_initialized, "UI already initialized");
  this->ini_path = Afk::get_absolute_path(".imgui.ini").string();
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
      Afk::get_absolute_path("res/font/NotoSans-Regular.ttf").string().c_str(),
      Ui::FONT_SIZE, nullptr, Afk::unicode_ranges.data());
  this->fonts["Noto Sans"] = noto_sans;

  auto *source_code_pro = io.Fonts->AddFontFromFileTTF(
      Afk::get_absolute_path("res/font/SourceCodePro-Regular.ttf").string().c_str(),
      Ui::FONT_SIZE, nullptr, Afk::unicode_ranges.data());
  this->fonts["Source Code Pro"] = source_code_pro;

  auto &style = ImGui::GetStyle();
  style.ScaleAllSizes(this->scale);
  this->is_initialized = true;
}

auto Ui::prepare() const -> void {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

auto Ui::draw() -> void {
  this->draw_stats();

  if (this->show_menu) {
    this->draw_menu_bar();
  }

  this->draw_about();
  this->draw_log();
  this->draw_model_viewer();
  this->draw_terrain_controller();
  this->draw_exit_screen();

  if (this->show_imgui) {
    ImGui::ShowDemoWindow(&this->show_imgui);
  }

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

auto Ui::draw_about() -> void {
  if (!this->show_about) {
    return;
  }

  ImGui::Begin("About", &this->show_about);
  ImGui::Text("afk engine version %s build %.6s (%s)", AFK_VERSION,
              GIT_HEAD_HASH, GIT_IS_DIRTY ? "dirty" : "clean");
  ImGui::Separator();
  ImGui::Text("%s", GIT_COMMIT_SUBJECT);
  ImGui::Text("Author: %s", GIT_AUTHOR_NAME);
  ImGui::Text("Date: %s", GIT_COMMIT_DATE);
  ImGui::End();
}

auto Ui::draw_menu_bar() -> void {
  // auto &afk = Engine::get();

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Tools")) {
      if (ImGui::MenuItem("Log")) {
        this->show_log = true;
      }
      if (ImGui::MenuItem("Model viewer")) {
        this->show_model_viewer = true;
      }
      if (ImGui::MenuItem("Terrain controller")) {
        this->show_terrain_controller = true;
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Difficulty")) {
      if (ImGui::MenuItem("Easy", nullptr, Afk::Engine::get().difficulty_manager.get_difficulty() == AI::DifficultyManager::Difficulty::EASY)) {
        if (Afk::Engine::get().difficulty_manager.get_difficulty() != AI::DifficultyManager::Difficulty::EASY) {
          Afk::Engine::get().difficulty_manager.set_difficulty(AI::DifficultyManager::Difficulty::EASY);
        }
      }
      if (ImGui::MenuItem("Normal", nullptr, Afk::Engine::get().difficulty_manager.get_difficulty() == AI::DifficultyManager::Difficulty::NORMAL)) {
        if (Afk::Engine::get().difficulty_manager.get_difficulty() != AI::DifficultyManager::Difficulty::NORMAL) {
          Afk::Engine::get().difficulty_manager.set_difficulty(AI::DifficultyManager::Difficulty::NORMAL);
        }
      }
      if (ImGui::MenuItem("Hard", nullptr, Afk::Engine::get().difficulty_manager.get_difficulty() == AI::DifficultyManager::Difficulty::HARD)) {
        if (Afk::Engine::get().difficulty_manager.get_difficulty() != AI::DifficultyManager::Difficulty::HARD) {
          Afk::Engine::get().difficulty_manager.set_difficulty(AI::DifficultyManager::Difficulty::HARD);
        }
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
      // afk.exit();
      this->show_exit_screen = true;
    }

    ImGui::EndMainMenuBar();
  }
}

auto Ui::draw_stats() -> void {
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

    ImGui::Text("%.1f fps (%.4f ms)", static_cast<double>(io.Framerate),
                static_cast<double>(io.Framerate) / 1000.0);
    ImGui::Separator();
    ImGui::Text("Position {%.1f, %.1f, %.1f}", static_cast<double>(pos.x),
                static_cast<double>(pos.y), static_cast<double>(pos.z));
    ImGui::Text("Angles   {%.1f, %.1f}", static_cast<double>(angles.x),
                static_cast<double>(angles.y));

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

auto Ui::draw_log() -> void {
  if (!this->show_log) {
    return;
  }

  ImGui::SetNextWindowSize({500, 400});
  this->log.draw("Log", &this->show_log);
}

auto Ui::draw_model_viewer() -> void {
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

auto Ui::draw_terrain_controller() -> void {
  if (!this->show_terrain_controller) {
    return;
  }

  ImGui::SetNextWindowSize({300, 150});

  if (ImGui::Begin("Terrain controller", &this->show_terrain_controller)) {
    // ImGui::SliderFloat("slider float", &f1, 0.0f, 1.0f, "ratio = %.3f");
  }
  ImGui::End();
}

auto Ui::draw_exit_screen() -> void {
  if (!this->show_exit_screen) {
    return;
  }

  auto &afk          = Engine::get();
  const auto texture = afk.renderer.get_texture("res/ui/exit.png");

  ImGui::SetNextWindowSize({525, 600});
  ImGui::Begin("Exit screen", &this->show_exit_screen);
  ImGui::Image(reinterpret_cast<void *>(texture.id),
               {static_cast<float>(texture.width), static_cast<float>(texture.height)});

  if (ImGui::Button("Exit")) {
    afk.exit();
  }

  ImGui::End();
}
