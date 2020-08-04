#include "afk/io/Log.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>

#include "afk/debug/Assert.hpp"

using afk::io::Log;

using std::ofstream;
using std::ostringstream;
using std::filesystem::path;
using namespace std::string_literals;

Log::Log() {
  auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  auto ss = ostringstream{};
  ss << ".log/" << std::put_time(std::localtime(&t), "%FT%H%M%S") << ".log";

  this->log_path = afk::io::get_absolute_path(ss.str());
  std::filesystem::create_directory(this->log_path.parent_path());
  this->log_file = ofstream{this->log_path};
  afk_assert(this->log_file.is_open(), "Failed to open log file");
}
