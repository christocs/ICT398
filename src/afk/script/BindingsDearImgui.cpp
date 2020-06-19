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

/**
 * lua's is_X functions are sometimes functions and sometimes macros
 * i would pass them to a function like lua_checkArgs
 * but with macros i can't do that
 * so this is also a macro
 */
#define CHECK_TYPE(lua, pos, fn_name, checker_fn)                              \
  if (!checker_fn(lua, pos)) {                                                 \
    const auto str = std::string{"Expected "} + #checker_fn +                  \
                     std::string{" at position "} + std::to_string(pos) +      \
                     std::string{" when calling "} + fn_name;                  \
    lua_pushstring(lua, str.c_str());                                          \
    lua_error(lua);                                                            \
  }                                                                            \
  0

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
  namespace Helper {
    /**
     * helper function for lua cfunctions
     */
    static auto lua_checkArgs(lua_State *lua, int required_args,
                              const std::string &function_name) -> bool {
      int n_args = lua_gettop(lua);
      if (n_args == required_args) {
        return true;
      }
      const auto err = (function_name + " expects 2 arguments, but recieved " +
                        std::to_string(n_args) + ".");
      lua_pushstring(lua, err.c_str());
      lua_error(lua);
      return false;
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

  /**
   * c-style lua funcs
   * extra testing scrutiny on these, requiring manual parameter checking
   */
  namespace CFuncs {
    /**
     * ideally i would have this as a normal C thing with std::string and bool* parameters
     * but luabridge is kinda borked and wouldn't let me use a bool* param
     */
    static auto Checkbox(lua_State *lua) -> int {
      Helper::lua_checkArgs(lua, 2, "Checkbox");
      CHECK_TYPE(lua, 1, "Checkbox", lua_isstring);
      CHECK_TYPE(lua, 2, "Checkbox", lua_isboolean);
      std::string name = lua_tostring(lua, 1);
      bool ref         = lua_toboolean(lua, 2);
      bool ret         = ImGui::Checkbox(name.c_str(), &ref);
      lua_pushboolean(lua, ret);
      lua_pushboolean(lua, ref);
      constexpr auto return_val_count = 2;
      return return_val_count;
    }
  }
}

auto Afk::add_imgui_bindings(lua_State *l) -> void {
  luabridge::getGlobalNamespace(l)
      .beginNamespace("imgui")
      .addFunction("begin", &ImBind::Begin)
      .addFunction("end_", &ImBind::End)
      .addFunction("text", &ImBind::Wrap::Fmt<ImGui::Text>)
      .addCFunction("checkbox", &ImBind::CFuncs::Checkbox)
      .addFunction("button", &ImBind::Button)
      .endNamespace();
}
