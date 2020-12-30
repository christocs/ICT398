#pragma once

#include <filesystem>

#include <nlohmann/json.hpp>

#include "afk/NumericTypes.hpp"

namespace afk {
  namespace io {
    using Json = nlohmann::json;

    constexpr i32 JSON_INDENTING = 2;
    auto read_json_from_file(const std::filesystem::path &file_path) -> Json;
    auto write_json_to_file(const std::filesystem::path &file_path, const Json &json) -> void;
  }
}
