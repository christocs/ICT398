#pragma once
#include <functional>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>
// nomove
#include "afk/script/LuaInclude.hpp"
// nomove
#include <LuaBridge/LuaBridge.h>

namespace Afk {
  /**
   * @brief Finite state machine
   */
  class StateMachine {
  public:
    StateMachine() = default;
    /**
     * Immediately fire a command
     */
    auto fire(const std::string &command) -> void;
    /**
     * Queue up a command
     */
    auto queue(const std::string &command) -> void;
    /**
     * Pump queued commands
     */
    auto pump() -> void;

  private:
    typedef std::function<void(void)> StateChangeCallback;

    std::queue<std::string> queued_commands = {};

    friend class StateMachineBuilder;
    struct TransitionBaseState {
      TransitionBaseState()     = default;
      std::string current_state = {};
      std::string command_name  = {};
      auto operator==(const TransitionBaseState &rhs) const -> bool;
    };
    struct Hasher {
      auto operator()(const TransitionBaseState &self) const -> std::size_t;
    };
    struct TransitionGoal {
      TransitionGoal()             = default;
      std::string end_state        = {};
      StateChangeCallback callback = {};
    };

    std::string current_state;
    std::unordered_map<TransitionBaseState, TransitionGoal, Hasher> transition_map = {};
    std::unordered_map<std::string, StateChangeCallback> on_enter = {};
    std::unordered_map<std::string, StateChangeCallback> on_exit  = {};
  };

  /**
   * @brief State Machine builder for Lua.
   * Define states with fluent-style syntax.
   *
   * Inspired by https://github.com/appccelerate/statemachine
   */
  class StateMachineBuilder {
  public:
    StateMachineBuilder() = default;
    /**
     * Set current state to be defined
     */
    auto in(const std::string &state) -> StateMachineBuilder &;
    /**
     * On entering the current state, do callback
     */
    auto on_enter(luabridge::LuaRef callback) -> StateMachineBuilder &;
    /**
     * On exiting the current state, do callback
     */
    auto on_exit(luabridge::LuaRef callback) -> StateMachineBuilder &;
    /**
     * On this specific command do something
     * \sa go
     * \sa call
     */
    auto on(const std::string &command) -> StateMachineBuilder &;
    /**
     * When the command is recieved, move to state
     * \param state state to move to
     */
    auto go(const std::string &state) -> StateMachineBuilder &;
    /**
     * When the command is recieved in this state, call the callback
     */
    auto call(luabridge::LuaRef callback) -> StateMachineBuilder &;
    /**
     * Set the initial state
     */
    auto initial_state(const std::string &state) -> StateMachineBuilder &;
    /**
     * Build the FSM
     */
    auto build() -> StateMachine;

  private:
    StateMachine machine        = {};
    std::string current_state   = {};
    std::string current_command = {};
  };
}
