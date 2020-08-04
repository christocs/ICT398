#include "afk/ui/Log.hpp"

using afk::ui::Log;

Log::Log() {
  this->clear();
}

auto Log::clear() -> void {
  this->buffer.clear();
  this->line_offsets.clear();
  this->line_offsets.push_back(0);
}

IM_FMTARGS(2) auto Log::append(const char *fmt, ...) -> void {
  auto old_size = this->buffer.size();
  va_list args;
  va_start(args, fmt);
  this->buffer.appendfv(fmt, args);
  va_end(args);

  for (auto new_size = this->buffer.size(); old_size < new_size; old_size++) {
    if (this->buffer[old_size] == '\n') {
      this->line_offsets.push_back(old_size + 1);
    }
  }
}

auto Log::draw(const char *title, bool *open) -> void {
  if (!ImGui::Begin(title, open)) {
    ImGui::End();
    return;
  }

  // Options menu
  if (ImGui::BeginPopup("Options")) {
    ImGui::Checkbox("Auto-scroll", &this->auto_scroll);
    ImGui::EndPopup();
  }

  // Main window
  if (ImGui::Button("Options")) {
    ImGui::OpenPopup("Options");
  }

  ImGui::SameLine();
  auto clear = ImGui::Button("Clear");
  ImGui::SameLine();
  auto copy = ImGui::Button("Copy");
  ImGui::SameLine();
  this->filter.Draw("Filter", -100.0f);

  ImGui::Separator();
  ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

  if (clear) {
    this->clear();
  }

  if (copy) {
    ImGui::LogToClipboard();
  }

  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
  const auto *buf     = this->buffer.begin();
  const auto *buf_end = this->buffer.end();
  if (this->filter.IsActive()) {
    for (int line_no = 0; line_no < this->line_offsets.Size; line_no++) {
      const auto *line_start = buf + this->line_offsets[line_no];
      const auto *line_end   = (line_no + 1 < this->line_offsets.Size)
                                 ? (buf + this->line_offsets[line_no + 1] - 1)
                                 : buf_end;

      if (this->filter.PassFilter(line_start, line_end)) {
        ImGui::TextUnformatted(line_start, line_end);
      }
    }
  } else {
    auto clipper = ImGuiListClipper{};

    clipper.Begin(this->line_offsets.Size);
    while (clipper.Step()) {
      for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++) {
        const auto *line_start = buf + this->line_offsets[line_no];
        const auto *line_end   = (line_no + 1 < this->line_offsets.Size)
                                   ? (buf + this->line_offsets[line_no + 1] - 1)
                                   : buf_end;
        ImGui::TextUnformatted(line_start, line_end);
      }
    }

    clipper.End();
  }

  ImGui::PopStyleVar();

  if (this->auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
    ImGui::SetScrollHereY(1.0f);
  }

  ImGui::EndChild();
  ImGui::End();
}
