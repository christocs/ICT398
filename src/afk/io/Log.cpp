#include "afk/io/Log.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>

#include "afk/debug/Assert.hpp"
#include "afk/io/Unicode.hpp"

using afk::io::Log;

using std::ofstream;
using std::ostringstream;
using std::filesystem::path;
using namespace std::string_literals;

auto Log::open_log_file() -> void {
  auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  auto ss = ostringstream{};
  ss << afk::io::to_cstr(this->LOG_DIR) << "/"
     << std::put_time(std::localtime(&t), "%FT%H%M%S") << ".txt";

  this->log_path = afk::io::get_resource_path(ss.str());
  this->log_file = ofstream{this->log_path};
  afk_assert(this->log_file.is_open(), "Failed to open log file");
}
