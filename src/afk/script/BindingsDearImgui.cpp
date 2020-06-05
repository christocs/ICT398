#include "BindingsDearImgui.hpp"
extern "C" {
#include <lua/lauxlib.h>
#include <lua/lua.h>
#include <lua/lualib.h>
}
#include <memory>
#include <string>
#include <vector>

#include <LuaBridge/LuaBridge.h>
#include <imgui/imgui.h>

using luabridge::LuaRef;

namespace ImBind {
  namespace Wrap {
    template<void (*Wrapped)(const char *fmt, ...)>
    static auto Fmt(const std::string &fmt) -> void {
      Wrapped("%s", fmt.c_str());
    }
  }

  static auto Begin(const std::string &name) -> void {
    ImGui::Begin(name.c_str());
  }
}

auto Afk::add_imgui_bindings(lua_State *l) -> void {
  luabridge::getGlobalNamespace(l)
      .beginNamespace("imgui")
      .addFunction("begin", &ImBind::Begin)
      .addFunction("end_", &ImGui::End)
      .addFunction("text", &ImBind::Wrap::Fmt<ImGui::Text>)
      .endNamespace();
}
