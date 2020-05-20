#include "afk/script/StateMachine.hpp"

using Afk::StateMachine;
using Afk::StateMachineBuilder;

auto StateMachineBuilder::in(const std::string &state) -> StateMachineBuilder & {
  this->current_state = state;
  return *this;
}
auto StateMachineBuilder::on_enter(luabridge::LuaRef callback) -> StateMachineBuilder & {
  this->machine.on_enter[this->current_state] = callback;
  return *this;
}
auto StateMachineBuilder::on_exit(luabridge::LuaRef callback) -> StateMachineBuilder & {
  this->machine.on_exit[this->current_state] = callback;
  return *this;
}
auto StateMachineBuilder::on(const std::string &command) -> StateMachineBuilder & {
  this->current_command = command;
  return *this;
}
auto StateMachineBuilder::go(const std::string &state) -> StateMachineBuilder & {
  this->machine.transition_map[{this->current_state, this->current_command}].end_state = state;
  return *this;
}
auto StateMachineBuilder::call(luabridge::LuaRef callback) -> StateMachineBuilder & {
  this->machine.transition_map[{this->current_state, this->current_command}].callback = callback;
  return *this;
}
auto StateMachineBuilder::initial_state(const std::string &state) -> StateMachineBuilder & {
  this->machine.current_state = state;
  return *this;
}
auto StateMachineBuilder::build() -> StateMachine {
  return this->machine;
}

auto StateMachine::fire(const std::string &command) -> void {
  auto transition = this->transition_map.find(
      StateMachine::TransitionBaseState{this->current_state, command});
  if (transition != this->transition_map.end()) {
    auto exit_callback  = this->on_exit.find(this->current_state);
    this->current_state = transition->second.end_state;
    auto enter_callback = this->on_enter.find(this->current_state);
    if (exit_callback != this->on_exit.end()) {
      exit_callback->second();
    }
    if (transition->second.callback) {
      transition->second.callback();
    }
    if (enter_callback != this->on_enter.end()) {
      enter_callback->second();
    }
  } else {
    throw std::runtime_error{"No transition was defined for receiving " +
                             command + " for state " + this->current_state};
  }
}

auto StateMachine::Hasher::operator()(const TransitionBaseState &self) const -> std::size_t {
  return std::hash<std::string>{}(self.current_state) ^
         std::hash<std::string>{}(self.command_name);
}

auto StateMachine::TransitionBaseState::operator==(const TransitionBaseState &rhs) const
    -> bool {
  return (this->command_name == rhs.command_name) &&
         (this->current_state == rhs.current_state);
}
