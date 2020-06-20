#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "afk/ai/Crowds.hpp"
#include "afk/ai/DifficultyManager.hpp"
#include "afk/ai/NavMeshManager.hpp"
#include "afk/event/EventManager.hpp"
#include "afk/physics/PhysicsBodySystem.hpp"
#include "afk/renderer/Camera.hpp"
#include "afk/renderer/Renderer.hpp"
#include "afk/terrain/TerrainManager.hpp"
#include "afk/ui/Ui.hpp"
#include "entt/entt.hpp"

struct lua_State;
namespace Afk {
  class Engine {
  public:
    static constexpr const char *GAME_NAME = "ICT397";

    Renderer renderer                   = {};
    EventManager event_manager          = {};
    Ui ui                               = {};
    Camera camera                       = {};
    TerrainManager terrain_manager      = {};
    AI::NavMeshManager nav_mesh_manager = {};
    AI::Crowds crowds                   = {};

    entt::registry registry;
    Afk::PhysicsBodySystem physics_body_system{glm::vec3(0.0f, -9.81f, 0.0f)};
    lua_State *lua = nullptr;

    GameObject camera_entity = registry.create();

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

    AI::DifficultyManager difficulty_manager = {};

  private:
    bool is_initialized = false;
    bool is_running     = true;
    int frame_count     = {};
    float last_update   = {};
  };
}
