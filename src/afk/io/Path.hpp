#pragma once

#include <filesystem>

namespace afk {
  namespace io {
    /**
     * Returns the equivalent absolute path of a specified relative resource
     * path. The specified path is relative to the executable directory.
     *
     * @param file_path The path relative to the executable directory.
     * @return The absolute path.
     */
    auto get_resource_path(const std::filesystem::path &file_path = "")
        -> std::filesystem::path;

    constexpr const auto ENGINE_DIRS = {
        u8"cfg",
        u8"log",
    };

    /**
     * Creates all the directories used by the engine.
     */
    auto create_engine_dirs() -> void;
  }
}
