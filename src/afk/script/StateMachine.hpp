#pragma once
#include <string>
#include <unordered_map>

class LuaRef {};
namespace Afk {
  /**
   * @brief State Machine class for Lua.
   * Define states with fluent-style syntax.
   * Example:
   * fsm.in("begin").on("start").go("started").exec(startup)
   * fsm.in("start").on("sleep").go("sleeping").on("power_off").go("end")
   * fsm.init("begin")
   * fsm.call("start")
   */
  class StateMachineBuilder {
  public:
    class StateMachine;
    StateMachineBuilder();
    auto in(std::string state) -> StateMachineBuilder &;
    auto on_enter(LuaRef callback) -> StateMachineBuilder &;
    auto on_exit(LuaRef callback) -> StateMachineBuilder &;
    auto on(std::string command) -> StateMachineBuilder &;
    auto go(std::string state) -> StateMachineBuilder &;
    auto exec(LuaRef callback) -> StateMachineBuilder &;

    auto initial_state(std::string state) -> StateMachineBuilder &;

    auto build() -> StateMachine;

    class StateMachine {
    public:
      auto fire(std::string command) -> void;

    private:
      struct TransitionBaseState {
        std::string current_state;
        std::string command_name;
        auto operator<(const TransitionBaseState &rhs) -> bool;
      };
      struct TransitionGoal {
        std::string end_state;
        LuaRef callback;
      };
      std::unordered_map<TransitionBaseState, TransitionGoal> transition_map;
      std::unordered_map<std::string, LuaRef> on_enter;
      std::unordered_map<std::string, LuaRef> on_exit;
    };

  private:
    std::string current_state;
    std::string current_command;
  };
}
