#include "afk/config/ConfigManager.hpp"

#include <filesystem>

#include "afk/config/Config.hpp"
#include "afk/debug/Assert.hpp"
#include "afk/io/Json.hpp"
#include "afk/io/JsonSerialization.hpp"
#include "afk/io/Log.hpp"
#include "afk/io/Path.hpp"
#include "afk/io/Unicode.hpp"

using afk::config::Config;
using afk::config::ConfigManager;
using afk::io::Json;

auto ConfigManager::load_config_file() -> void {
  const auto config_path =
      afk::io::get_resource_path(afk::io::to_cstr(this->CONFIG_FILE_PATH));

  if (!std::filesystem::exists(config_path.parent_path())) {
    std::filesystem::create_directory(config_path.parent_path());
  }

  if (!std::filesystem::exists(config_path)) {
    auto json = Json{};
    json      = this->config;
    afk::io::write_json_to_file(config_path, json);
  } else {
    auto json    = afk::io::read_json_from_file(config_path);
    this->config = json.get<Config>();
  }
}

auto ConfigManager::initialize() -> void {
  afk_assert(!this->is_initialized, "Config manager already initialized");
  this->load_config_file();
  this->is_initialized = true;
}
