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
    ScriptsComponent(GameObject e);
    auto check_live_reload(lua_State *l) -> void;
    auto add_script(const path &script_path, lua_State *l, EventManager *evt)
        -> ScriptsComponent &;
    auto remove_script(const path &script_path) -> void;
    auto get_script_table(const path &script_path, lua_State *lua) -> LuaRef;

  private:
    std::map<path, std::shared_ptr<Afk::LuaScript>> loaded_files;
    std::map<path, std::filesystem::file_time_type> last_write;
  };
}
