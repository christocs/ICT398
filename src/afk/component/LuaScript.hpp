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
  class ScriptsComponent;
  /**
   * Lua callback registered to the event manager
   */
  struct RegisteredLuaCall {
    Afk::Event::Type event_type;
    Afk::EventManager::Callback callback;
  };
  /**
   * Lua script
   */
  class LuaScript {
  public:
    LuaScript(EventManager *events, lua_State *lua, ScriptsComponent *owner);
    /**
     * This class is used in luabridge which requires a copy ctor
     * \todo Maybe see if there's some other way to architecture this. I think I could give luabridge a pointer instead
     */
    LuaScript(const LuaScript &other);
    /**
     * This class is used in luabridge which requires a move ctor
     */
    LuaScript(LuaScript &&other);
    /**
     * This class is used in luabridge which requires a copy assignment
     */
    auto operator=(const LuaScript &other) -> LuaScript &;
    /**
     * This class is used in luabridge which requires a move assignment
     */
    auto operator=(LuaScript &&other) -> LuaScript &;
    ~LuaScript();
    auto load(const std::filesystem::path &filename) -> void;
    auto unload() -> void;
    /**
     * Allows script to register its functions into the event system
     */
    auto register_fn(int event_val, LuaRef func) -> void;

    /**
     * The lua state
     */
    lua_State *lua;
    /**
     * owning data
     */
    ScriptsComponent *my_owner;

  private:
    EventManager *event_manager;
    std::shared_ptr<std::vector<RegisteredLuaCall>> registered_events;
  };
}
