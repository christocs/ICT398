#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include "afk/Afk.hpp"
#include "afk/io/Path.hpp"
#include "afk/ui/Log.hpp"

namespace afk {
  namespace io {
    /**
     * Logging
     */
    struct Log {
      std::filesystem::path log_path = {};
      std::ofstream log_file         = {};

      Log();
    };
    /**
     * Log things
     * \param value thing to log
     * \param log logger
     */
    template<typename T>
    auto operator<<(Log &log, T const &value) -> Log & {
      using std::ostringstream;

      auto &afk = Engine::get();
      auto ss   = ostringstream{};

      ss << value;
      afk.ui_manager.log.append("%s", ss.str().c_str());
      log.log_file << value;
      std::cout << value;

      return log;
    }

    inline auto log = Log{};
  }
}
