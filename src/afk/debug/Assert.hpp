#pragma once

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

#include "afk/io/Log.hpp"
#include "afk/NumericTypes.hpp"

#if defined(__GNUC__) || defined(__clang__)
  #define AFK_FUNCTION __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
  #define AFK_FUNCTION __FUNCSIG__
#else
  #define AFK_FUNCTION __func__
#endif

#define afk_assert(expression, message)                                        \
  afk::assertion(expression, message, #expression, __FILE__, __LINE__, AFK_FUNCTION)

#define afk_unreachable() afk::unreachable(__FILE__, __LINE__, AFK_FUNCTION)

#ifdef NDEBUG
  #define afk_assert_debug(expression, message) ((void)0)
#else
  #define afk_assert_debug(expression, message)                                \
    afk::assertion(expression, message, #expression, __FILE__, __LINE__, AFK_FUNCTION)
#endif

#ifdef NDEBUG
  #define afk_unreachable_debug() ((void)0)
#else
  #define afk_unreachable_debug()                                              \
    afk::unreachable(__FILE__, __LINE__, AFK_FUNCTION)
#endif

namespace afk {
  auto assertion(bool condition, const std::string &msg,
                 const std::string &expression, const std::string &file_name,
                 usize line_num, const std::string &function_name) -> void;
  [[noreturn]] auto unreachable(const std::string &file_name, usize line_num,
                                const std::string &function_name) -> void;
}

inline auto afk::assertion(bool condition, const std::string &msg,
                           const std::string &expression, const std::string &file_path,
                           usize line_num, const std::string &function_name) -> void {
  if (!condition) {
    const auto error = "\nAssertion '" + expression + "' failed: " + msg +
                       "\n  in " + function_name + "\n  at " + file_path + ":" +
                       std::to_string(line_num) + "\n  ";
    afk::io::log << msg << '\n';
    throw std::runtime_error{error};
  }
}

inline auto afk::unreachable(const std::string &file_path, usize line_num,
                             const std::string &function_name) -> void {

  const auto error = "\nUnreachable statement hit\n  in " + function_name +
                     "\n  at " + file_path + ":" + std::to_string(line_num) + "\n  ";

  afk::io::log << error << '\n';
  throw std::runtime_error{error};
}
