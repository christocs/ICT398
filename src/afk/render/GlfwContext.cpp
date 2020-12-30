#include "afk/render/GlfwContext.hpp"

#include <glad/glad.h>

#include "afk/debug/Assert.hpp"
// Must be loaded after GLAD.
#include <GLFW/glfw3.h>

using afk::render::GlfwContext;

GlfwContext::GlfwContext() {
  afk_assert(glfwInit(), "Failed to initialize GLFW");
}

GlfwContext::~GlfwContext() {
  glfwTerminate();
}
