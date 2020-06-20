#pragma once

struct lua_State;
namespace Afk {
  /**
   * Add bindings for the engine into lua
   */
  auto add_engine_bindings(lua_State *l) -> void;
}
