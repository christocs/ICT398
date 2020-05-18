#pragma once
#include <functional>
#include <map>
#include <string>
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
    auto fire(std::string command) -> void;

  private:
    typedef std::function<void(void)> StateChangeCallback;

    friend class StateMachineBuilder;
    std::string current_state;
    struct TransitionBaseState {
      TransitionBaseState()     = default;
      std::string current_state = {};
      std::string command_name  = {};
      auto operator<(const TransitionBaseState &rhs) const -> bool;
      auto operator==(const TransitionBaseState &rhs) const -> bool;
    };
    struct TransitionGoal {
      TransitionGoal()             = default;
      std::string end_state        = {};
      StateChangeCallback callback = {};
    };
    std::map<TransitionBaseState, TransitionGoal> transition_map = {};
    std::map<std::string, StateChangeCallback> on_enter          = {};
    std::map<std::string, StateChangeCallback> on_exit           = {};
  };

  /**
   * @brief State Machine builder for Lua.
   * Define states with fluent-style syntax.
   * Example:
   * fsm.in("begin").on("start").go("started").call(startup)
   * fsm.in("start").on("sleep").go("sleeping").on("power_off").go("end")
   * fsm.init("begin")
   * fsm.call("start")
   *
   * Inspired by https://github.com/appccelerate/statemachine
   */
  class StateMachineBuilder {
  public:
    StateMachineBuilder() = default;
    auto in(std::string state) -> StateMachineBuilder &;
    auto on_enter(luabridge::LuaRef callback) -> StateMachineBuilder &;
    auto on_exit(luabridge::LuaRef callback) -> StateMachineBuilder &;
    auto on(std::string command) -> StateMachineBuilder &;
    auto go(std::string state) -> StateMachineBuilder &;
    auto call(luabridge::LuaRef callback) -> StateMachineBuilder &;

    auto initial_state(std::string state) -> StateMachineBuilder &;

    auto build() -> StateMachine;

  private:
    StateMachine machine        = {};
    std::string current_state   = {};
    std::string current_command = {};
  };
}
