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
    auto fire(const std::string &command) -> void;
    auto queue(const std::string &command) -> void;
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
    auto in(const std::string &state) -> StateMachineBuilder &;
    auto on_enter(luabridge::LuaRef callback) -> StateMachineBuilder &;
    auto on_exit(luabridge::LuaRef callback) -> StateMachineBuilder &;
    auto on(const std::string &command) -> StateMachineBuilder &;
    auto go(const std::string &state) -> StateMachineBuilder &;
    auto call(luabridge::LuaRef callback) -> StateMachineBuilder &;

    auto initial_state(const std::string &state) -> StateMachineBuilder &;

    auto build() -> StateMachine;

  private:
    StateMachine machine        = {};
    std::string current_state   = {};
    std::string current_command = {};
  };
}
