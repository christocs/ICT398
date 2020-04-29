#pragma once

#include <filesystem>
#include <string>
extern "C" {
#include <lua/lauxlib.h>
#include <lua/lua.h>
#include <lua/lualib.h>
}
#include <LuaBridge/LuaBridge.h>

#include "afk/event/Event.hpp"
#include "afk/event/EventManager.hpp"

using luabridge::LuaRef;

namespace Afk {
  // Script needs to remember what calls it has registered to the event manager
  // so it can get rid of them later
  struct RegisteredLuaCall {
    Afk::Event::Type event_type;
    Afk::EventManager::Callback lua_ref;
  };

  class LuaScript {
  public:
    LuaScript(EventManager *event_manager);
    ~LuaScript();
    auto load(const std::filesystem::path &filename, lua_State *lua) -> void;
    auto unload() -> void;
    static auto setup_lua_state(lua_State *lua) -> void;
    auto register_fn(Afk::Event::Type event_type, LuaRef func) -> void;

  private:
    EventManager *event_manager;
    std::vector<RegisteredLuaCall> registered_events;
  };
}
