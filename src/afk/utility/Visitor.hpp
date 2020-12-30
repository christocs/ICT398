#pragma once

namespace afk {
  namespace utility {
    template<class... T>
    struct Visitor : T... {
      using T::operator()...;
    };

    template<class... T>
    Visitor(T...)->Visitor<T...>;
  }
}
