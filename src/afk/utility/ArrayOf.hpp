#pragma once

#include <array>

namespace afk {
  namespace utility {
    /**
     * Creates a constexpr array from the specified elements. Does not require
     * the number of elements.
     *
     * @tparam V The array value.
     * @tparam T The array type.
     * @param t The array vaue.
     * @return A constexpr array from the specified elements.
     */
    template<typename V, typename... T>
    constexpr auto array_of(T &&... t) -> std::array<V, sizeof...(T)> {
      return {{std::forward<T>(t)...}};
    }
  }
}
