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

    /** The rendering subsystem. */
    render::Renderer renderer = {};
    /** The event subsystem. */
    event::EventManager event_manager = {};
    /** The UI subsystem. */
    ui::UiManager ui_manager = {};
    /** The camera subsystem. */
    render::Camera camera = {};
    /** The ECS subsystem. */
    entt::registry registry = entt::registry{};
    /** Physics subsystem. */
    physics::PhysicsBodySystem physics_body_system = {};

    Engine()               = default;
    ~Engine()              = default;
    Engine(Engine &&)      = delete;
    Engine(const Engine &) = delete;
    auto operator=(const Engine &) -> Engine & = delete;
    auto operator=(Engine &&) -> Engine & = delete;

    /**
     * Returns a reference to the current engine instance.
     *
     * @return Returns a reference to the current engine instance.
     */
    static auto get() -> Engine &;

    /**
     * Initializes the afk engine.
     */
    auto initialize() -> void;

    /**
     * Draws one frame and swaps the front and back framebuffer.
     */
    auto render() -> void;

    /**
     * Advances the game simulation for one tick.
     */
    auto update() -> void;

    /**
     * Exits the engine.
     */
    auto exit() -> void;

    /**
     * Returns the current time in seconds.
     *
     * The current time is counted since the start of the engine.
     *
     * @return Returns the current time in seconds.
     */
    auto static get_time() -> f32;

    /**
     * Returns the delta time of the last update in seconds.
     *
     * @return Returns the delta time of the last update in seconds.
     */
    auto get_delta_time() -> f32;

    /**
     * Returns if the engine is running.
     *
     * @return True if the engine is running.
     */
    auto get_is_running() const -> bool;

    /**
     * Handles the mouse being moved.
     * @todo Move into an input manager
     *
     * @param event The mouse move event.
     */
    auto move_mouse(event::Event event) -> void;

    /**
     * Handles a key being pressed.
     * @todo Move into an input manager
     *
     * @param event The key press event.
     */
    auto move_keyboard(event::Event event) -> void;

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
