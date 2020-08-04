#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "afk/Afk.hpp"

auto main([[maybe_unused]] i32 argc, [[maybe_unused]] char **argv) -> i32 {
  auto &afk = afk::Engine::get();

  afk.initialize();

  while (afk.get_is_running()) {
    afk.update();
    afk.render();
  }

  return EXIT_SUCCESS;
}
