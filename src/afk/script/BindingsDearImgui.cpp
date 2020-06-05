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
#include <glm/glm.hpp>
#include <imgui/imgui.h>

using luabridge::LuaRef;

/**
 * ImGui binding facade
 */
namespace ImBind {
  namespace Wrap {
    /**
     * mmmm templates
     */
    template<void (*Wrapped)(const char *fmt, ...)>
    static auto Fmt(const std::string &fmt) -> void {
      Wrapped("%s", fmt.c_str());
    }
  }

  static auto Begin(const std::string &name) -> void {
    ImGui::Begin(name.c_str());
  }

  static auto End() -> void {
    ImGui::End();
  }

  static auto to_imvec2(glm::vec2 in) -> ImVec2 {
    return ImVec2{in.x, in.y};
  }

  static auto Button(const std::string &name, const glm::vec2 &size) -> bool {
    return ImGui::Button(name.c_str(), to_imvec2(size));
  }
}

auto Afk::add_imgui_bindings(lua_State *l) -> void {
  luabridge::getGlobalNamespace(l)
      .beginNamespace("imgui")
      .addFunction("begin", &ImBind::Begin)
      .addFunction("end_", &ImBind::End)
      .addFunction("text", &ImBind::Wrap::Fmt<ImGui::Text>)
      .addFunction("button", &ImBind::Button)
      .endNamespace();
}
