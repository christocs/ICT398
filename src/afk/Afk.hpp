#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "afk/NumericTypes.hpp"
#include "afk/event/EventManager.hpp"
#include "afk/physics/PhysicsBodySystem.hpp"
#include "afk/render/Camera.hpp"
#include "afk/render/Renderer.hpp"
#include "afk/ui/UiManager.hpp"
#include "entt/entt.hpp"

namespace afk {
  /**
   * Encapsulates the afk engine state.
   */
  class Engine {
  public:
    /** The game name, this is displayed as the window title. */
    static constexpr const char8_t *GAME_NAME = u8"ICT397";

    /** rendering subsystem. */
    render::renderer renderer = {};
    /** event subsystem. */
    event::eventManager event_manager = {};
    /** UI subsystem. */
    ui::UiManager ui_manager = {};
    /** Camera subsystem. */
    render::Camera camera = {};
    /** ECS subsystem. */
    entt::registry registry = entt::registry{};
    /** Physics subsystem. */
    physics::PhysicsBodySystem physics_body_system = {};

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

    auto move_mouse(event::Event event) -> void;
    auto move_keyboard(event::Event event) -> void;

    auto static get_time() -> f32;
    auto get_delta_time() -> f32;
    auto get_is_running() const -> bool;

  private:
    /** Is the engine initialized? */
    bool is_initialized = false;
    /** Is the engine running? */
    bool is_running = true;
    /** The number of frames rendered since the engine started. */
    i32 frame_count = {};
    /** The time, in seconds, since the last update. */
    f32 last_update = {};
  };
}
