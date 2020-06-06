#include <filesystem>
#include <map>
#include <memory>

#include "afk/component/BaseComponent.hpp"
#include "afk/component/LuaScript.hpp"

using std::filesystem::path;

namespace Afk {

  /**
   * Gets around ENTT's limitation of one component of a type per entity.
   * (With other ECS like systems, each unique script would get it's own type)
   * (However, we can't really do this when it's all in Lua)
   * (So all the scripts are bundled into one ScriptsComponent)
   */
  class ScriptsComponent : public BaseComponent {
  public:
    ScriptsComponent(GameObject e, lua_State *lua);
    auto check_live_reload() -> void;
    auto add_script(const path &script_path, EventManager *evt) -> ScriptsComponent &;
    auto remove_script(const path &script_path) -> void;
    /**
     * for lua
     */
    auto get_script_table(const std::string &script_path) -> LuaRef;

    /**
     * allows components to use other components data without relying on a specific load order
     * do not index this map with []. you will get compile errors (LuaRefs are not default constructible)
     */
    std::map<path, LuaRef> global_tables;

  private:
    std::map<path, std::shared_ptr<Afk::LuaScript>> loaded_files = {};
    std::map<path, std::filesystem::file_time_type> last_write   = {};
    lua_State *lua;
  };
}
