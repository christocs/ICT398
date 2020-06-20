#pragma once

struct lua_State;
namespace Afk {
  /**
   * Add bindings for imgui to the engine
   */
  auto add_imgui_bindings(lua_State *l) -> void;
}
