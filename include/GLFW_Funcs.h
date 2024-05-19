#pragma once

#include <string>

#include "Global_Macros.h"
#include "Utilities.h"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <cstdio>

ENGINE_NAMESPACE_BEGIN
namespace GLFW_Types {

typedef void (*Set_Graphics_Viewport_T)(int, int, int, int);
static Set_Graphics_Viewport_T __Set_Graphics_Viewport_Cb = nullptr;
typedef GLFWwindow GLFW_OSWindow_T;

}; // namespace GLFW_Types

namespace GLFW_Funcs {

static void glfw_error_callback(int error, const char *description) {
  write_log(description);
  throw("GLFW error, see common_logs file");
}

static void glfw_window_close_callback(GLFWwindow *window) {
  write_log("GLFW: Window Closed");
}

static void glfw_framebuffer_size_callback(GLFWwindow *window, int w, int h) {
  /* __Set_Graphics_Viewport_Cb(0, 0, w, h); */
}

// CALLBACKS -- END

/* TYPES */

/* static void Init(Set_Graphics_Viewport_Type set_graphics_viewport_cb) { */
// Used in glfw_framebuffer_size_callback function
/* if (__Set_Graphics_Viewport_Cb != nullptr) */
/*   __Set_Graphics_Viewport_Cb = set_graphics_viewport_cb; */

static void GLFW_Init() {
  int success = glfwInit();
  if (!success) {
    write_log("GLFW Error: Couldn't initiate GLFW");
  };

  glfwSetErrorCallback(glfw_error_callback);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

static void GLFW_CreateWindow(GLFWwindow **glfw_window, std::string &title,
                              uint16_t w, uint16_t h) {
  *glfw_window = glfwCreateWindow((int)w, (int)h, title.c_str(), NULL, NULL);

  if (!glfw_window) {
    write_log("Failed to create GLFW Window");
    glfwTerminate();
  }

  glfwMakeContextCurrent(*glfw_window);
  glfwSetWindowCloseCallback(*glfw_window, glfw_window_close_callback);
  /* glfwSetFramebufferSizeCallback(*glfw_window,
   * glfw_framebuffer_size_callback); */
}

static void GLFW_WindowLoop(GLFWwindow *glfw_window
                            /* , void(*gameloop_cb) */
) {
  while (!glfwWindowShouldClose(glfw_window)) {
    glfwSwapBuffers(glfw_window);
    glfwPollEvents();
  }
}

static void GLFW_Terminate() { glfwTerminate(); }

// PURE GLFW FUNCS
static GLFWglproc Get_Proc_Address(const char *procname) {
  return glfwGetProcAddress(procname);
};

}; // namespace GLFW_Funcs
ENGINE_NAMESPACE_END
