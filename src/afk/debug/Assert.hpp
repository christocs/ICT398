#pragma once

#include <cstdlib>
#include <iostream>
#include <string>

#include "afk/NumericTypes.hpp"

#if defined(__GNUC__) || defined(__clang__)
  #define AFK_FUNCTION __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
  #define AFK_FUNCTION __FUNCSIG__
#else
  #define AFK_FUNCTION __func__
#endif

#define afk_assert(expression, msg)                                            \
  afk::assertion(expression, msg, #expression, __FILE__, __LINE__, AFK_FUNCTION)

#define afk_unreachable() afk::unreachable(__FILE__, __LINE__, AFK_FUNCTION)

#ifdef NDEBUG
  #define afk_assert_debug(expression, msg) ((void)0)
#else
  #define afk_assert_debug(expression, msg)                                    \
    afk::assertion(expression, msg, #expression, __FILE__, __LINE__, AFK_FUNCTION)
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

  /**
   * Performs a checked runtime assertion which runs on all builds. If the
   * assertion fails std::abort is called.
   *
   * @param condition The assertion condition.
   * @param msg The assertion failure message.
   * @param expression The literal assertion condition.
   * @param file_path The name of the file containing the assertion.
   * @param line_num The line of the assertion in said file.
   * @param function_name The name of the function containing the assertion.
   */
  inline auto assertion(bool condition, const std::string &msg,
                        const std::string &expression, const std::string &file_path,
                        usize line_num, const std::string &function_name) -> void {
    using namespace std::string_literals;

    if (!condition) {
      const auto error = "\nAssertion '"s + expression + "' failed: "s + msg +
                         "\n  in "s + function_name + "\n  at "s + file_path +
                         ":"s + std::to_string(line_num) + "\n  "s;

      std::cerr << error;
      std::abort();
    }
  }

  /**
   * Denotes an unreachable statement. If this statement is hit, std::abort
   * is called.
   *
   * @param file_path The name of the file containing the assertion.
   * @param line_num The line of the assertion in said file.
   * @param function_name The name of the function containing the assertion.
   */
  inline auto unreachable(const std::string &file_path, usize line_num,
                          const std::string &function_name) -> void {
    using namespace std::string_literals;

    const auto error = "\nUnreachable statement hit\n  in "s + function_name +
                       "\n  at "s + file_path + ":"s + std::to_string(line_num) + "\n  "s;

    std::cerr << error;
    std::abort();
  }
}
