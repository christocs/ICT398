#pragma once

namespace afk {
  namespace render {
    /**
     * Handles constructing and destructing a GLFW context.
     */
    struct GlfwContext {
      GlfwContext();
      ~GlfwContext();
    };
  }
}
