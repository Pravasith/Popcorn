#pragma once

#include "Callbacks.h"
#include <GLFW/glfw3.h>

namespace GLFW_Funcs {
static void Init() {
  glfwInit();
  glfwSetErrorCallback(glfw_error_callback);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

static void Set_Framebuffer_Size_Callback(GLFWwindow *glfw_window) {
  glfwSetFramebufferSizeCallback(glfw_window, glfw_framebuffer_size_callback);
}

static void Create_Window(GLFWwindow *glfw_window, const char *dir) {
  glfw_window = glfwCreateWindow(800, 600, dir, NULL, NULL);

  if (!glfw_window) {
    write_log("Failed to create GLFW Window");
    glfwTerminate();
  }

  glfwMakeContextCurrent(glfw_window);
  glfwSetWindowCloseCallback(glfw_window, glfw_window_close_callback);
  glfwSetFramebufferSizeCallback(glfw_window, glfw_framebuffer_size_callback);
}

// PURE GLFW FUNCS
static GLFWglproc Get_Proc_Address(const char *procname) {
  return glfwGetProcAddress(procname);
};

}; // namespace GLFW_Funcs
