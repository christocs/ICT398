#pragma once

#include <array>

namespace Afk {
  /**
   * Clean way to create an array of T without needing the number of elements
   */
  template<typename V, typename... T>
  constexpr auto array_of(T &&... t) -> std::array<V, sizeof...(T)> {
    return {{std::forward<T>(t)...}};
  }
}
