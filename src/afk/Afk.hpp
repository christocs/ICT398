#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "afk/event/EventManager.hpp"
#include "afk/renderer/Camera.hpp"
#include "afk/renderer/Renderer.hpp"
#include "afk/ui/Ui.hpp"
#include "entt/entt.hpp"

struct lua_State;
namespace Afk {
  class Engine {
  public:
    static constexpr const char *GAME_NAME = "ICT397";

    Renderer renderer          = {};
    EventManager event_manager = {};
    Ui ui                      = {};
    Camera camera              = {};
    entt::registry registry    = entt::registry{};

    Engine()               = default;
    ~Engine()              = default;
    Engine(Engine &&)      = delete;
    Engine(const Engine &) = delete;
    auto operator=(const Engine &) -> Engine & = delete;
    auto operator=(Engine &&) -> Engine & = delete;

    static auto get() -> Engine &;

    auto exit() -> void;
    auto initialize() -> void;
    auto render() -> void;
    auto update() -> void;

    auto static get_time() -> float;
    auto get_delta_time() -> float;
    auto get_is_running() const -> bool;

  private:
    bool is_initialized = false;
    bool is_running     = true;
    int frame_count     = {};
    float last_update   = {};
  };
}
