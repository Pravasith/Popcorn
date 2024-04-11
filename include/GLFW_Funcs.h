#pragma once

#include "Utilities.h"
#include <GLFW/glfw3.h>
#include <cstdio>

namespace GLFW_Funcs {

// CALLBACKS -- START
typedef void (*Set_Graphics_Viewport_Type)(int, int, int, int);
static Set_Graphics_Viewport_Type __Set_Graphics_Viewport_Cb = nullptr;

static void glfw_error_callback(int error, const char *description) {
  fprintf(stderr, "Error: %s\n", description);
  write_log(description);
  throw("glfw error");
}

static void glfw_window_close_callback(GLFWwindow *window) {
  write_log("Window Closed");
}

static void glfw_framebuffer_size_callback(GLFWwindow *window, int w, int h) {
  __Set_Graphics_Viewport_Cb(0, 0, w, h);
}
// CALLBACKS -- END

static void Init(Set_Graphics_Viewport_Type set_graphics_viewport_cb) {
  glfwInit();
  glfwSetErrorCallback(glfw_error_callback);

  // Used in glfw_framebuffer_size_callback function
  if (__Set_Graphics_Viewport_Cb != nullptr)
    __Set_Graphics_Viewport_Cb = set_graphics_viewport_cb;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
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
