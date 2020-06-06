#include "LuaScript.hpp"

#include "afk/component/ScriptsComponent.hpp"
#include "afk/debug/Assert.hpp"
#include "afk/event/EventManager.hpp"
#include "afk/io/Path.hpp"
#include "afk/script/Bindings.hpp"
#include "afk/script/Script.hpp"

using namespace std::string_literals;
using Afk::LuaScript;

auto LuaScript::register_fn(int event_val, LuaRef func) -> void {
  auto event_type = static_cast<Afk::Event::Type>(event_val);
  this->registered_events->push_back(RegisteredLuaCall{
      event_type, Afk::EventManager::Callback{std::function<void(Afk::Event)>{func}}});
  auto &evt = this->registered_events->at(this->registered_events->size() - 1);
  event_manager->register_event(event_type, evt.callback);
}

auto LuaScript::load(const std::filesystem::path &filename) -> void {
  this->unload();
  luaL_loadfile(this->lua, filename.string().c_str()); // stack: FILE

  auto f = this->my_owner->global_tables.find(filename);
  if (f != this->my_owner->global_tables.end()) {
    f->second.push();
  } else {
    auto new_tbl = LuaRef::newTable(this->lua);
    this->my_owner->global_tables.emplace(filename, new_tbl);
    new_tbl.push();
  } // stack: FILE ENV

  luabridge::push(this->lua, this);    // stack: FILE ENV THIS
  lua_setfield(this->lua, -2, "this"); // stack: FILE ENV

  lua_newtable(this->lua);                // stack: FILE ENV METATABLE
  lua_getglobal(this->lua, "_G");         // stack: FILE ENV METATABLE GLOBAL
  lua_setfield(this->lua, -2, "__index"); // stack: FILE ENV METATABLE
  lua_setmetatable(this->lua, -2);

  lua_setupvalue(this->lua, -2, 1); // stack: FILE

  auto lua_ret = lua_pcall(this->lua, 0, 0, 0);

  if (lua_ret != 0) {
    throw std::runtime_error{"Error loading "s + filename.string() + ": "s +
                             lua_tostring(this->lua, -1)};
  }
}

auto LuaScript::unload() -> void {
  while (this->registered_events != nullptr && this->registered_events->size() > 0) {
    auto &evt = this->registered_events->at(this->registered_events->size() - 1);
    event_manager->deregister_event(evt.event_type, evt.callback);
    this->registered_events->erase(this->registered_events->end() - 1);
  }
}

LuaScript::LuaScript(Afk::EventManager *events, lua_State *lua_state, ScriptsComponent *owner)
  : lua(lua_state), my_owner(owner), event_manager(events),
    registered_events(std::make_shared<std::vector<Afk::RegisteredLuaCall>>()) {
  afk_assert(event_manager != nullptr, "Event manager must not be null.");
}
LuaScript::~LuaScript() {
  this->unload();
}

auto LuaScript::operator=(LuaScript &&other) -> LuaScript & {
  this->lua               = other.lua;
  this->my_owner          = other.my_owner;
  this->event_manager     = other.event_manager;
  this->registered_events = std::move(other.registered_events);
  return *this;
}
LuaScript::LuaScript(LuaScript &&other) {
  this->lua               = other.lua;
  this->my_owner          = other.my_owner;
  this->event_manager     = other.event_manager;
  this->registered_events = std::move(other.registered_events);
}

auto LuaScript::operator=(const LuaScript &other) -> LuaScript & {
  this->lua               = other.lua;
  this->my_owner          = other.my_owner;
  this->event_manager     = other.event_manager;
  this->registered_events = other.registered_events;
  return *this;
}
LuaScript::LuaScript(const LuaScript &other) {
  this->lua               = other.lua;
  this->my_owner          = other.my_owner;
  this->event_manager     = other.event_manager;
  this->registered_events = other.registered_events;
}
