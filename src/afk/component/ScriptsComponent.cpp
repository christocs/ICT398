#include "ScriptsComponent.hpp"

#include <string>

#include "afk/Afk.hpp"
#include "afk/io/Log.hpp"
#include "afk/io/Path.hpp"

using Afk::ScriptsComponent;

ScriptsComponent::ScriptsComponent(GameObject e, lua_State *lua_state)
  : loaded_files(), last_write(), lua(lua_state) {
  this->owning_entity = e;
}

auto ScriptsComponent::add_script(const path &script_path, EventManager *evt_mgr)
    -> ScriptsComponent & {
  const auto abs_path = Afk::get_absolute_path(script_path);
  auto lua_script = std::shared_ptr<LuaScript>(new LuaScript{evt_mgr, this->lua, this});
  lua_script->load(abs_path);
  this->loaded_files.emplace(abs_path, lua_script);
  this->last_write.emplace(abs_path, std::filesystem::last_write_time(abs_path));
  return *this;
}
auto ScriptsComponent::remove_script(const path &script_path) -> void {
  const auto abs_path = Afk::get_absolute_path(script_path);
  this->loaded_files.erase(abs_path);
}

auto ScriptsComponent::get_script_table(const std::string &script_path) -> LuaRef {
  const auto full_path = Afk::get_absolute_path(script_path);
  auto f               = this->global_tables.find(full_path);
  if (f != this->global_tables.end()) {
    return f->second;
  }
  auto new_tbl = LuaRef::newTable(this->lua);
  this->global_tables.emplace(full_path, new_tbl);
  return new_tbl;
}

auto ScriptsComponent::check_live_reload() -> void {
  for (auto &script : this->loaded_files) {
    const auto &script_path = script.first;
    auto recent_write       = std::filesystem::last_write_time(script_path);
    if (recent_write > this->last_write[script_path]) {
      script.second->unload();
      script.second->load(script_path);
      this->last_write[script_path] = recent_write;
    }
  }
}
