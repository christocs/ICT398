#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include "afk/Engine.hpp"
#include "afk/io/Path.hpp"
#include "afk/ui/Log.hpp"

namespace afk {
  namespace io {
    /**
     * Handles logging throughout the engine. Log entries are written to
     * the log file, printed to standard out, and printed to the in game log.
     */
    struct Log {
      static constexpr const auto *LOG_DIR = u8"log";

      /** The path to the current log file. */
      std::filesystem::path log_path = {};
      /** The log file handle. */
      std::ofstream log_file = {};

      /**
       * Opens the log file.
       */
      auto open_log_file() -> void;
    };

    /**
     * Logs the specified value to the specified log.
     * @param value The value to log.
     * @param log The log to use.
     */
    template<typename T>
    auto operator<<(Log &log, T const &value) -> Log & {
      using std::ostringstream;

      auto &afk = Engine::get();
      auto ss   = ostringstream{};

      if (!log.log_file.is_open()) {
        log.open_log_file();
      }

      ss << value;
      afk.ui_manager.log.append("%s", ss.str().c_str());
      log.log_file << value;
      std::cout << value;

      return log;
    }

    /** The default logger. */
    inline auto log = Log{};
  }
}
