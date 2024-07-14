#pragma once

#include <string>

#include "Global_Macros.h"
#include "Utilities.h"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <cstdio>

ENGINE_NAMESPACE_BEGIN
namespace GLFW_Types {

typedef GLFWwindow GLFW_OSWindow_T;

}; // namespace GLFW_Types

namespace GLFW_Funcs {

static void glfw_error_callback(int error, const char *description) {
  write_log(description);
  throw("GLFW error, see common_logs file");
}

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

static void GLFW_CreateWindow(GLFWwindow **glfwWindow, std::string &title,
                              uint16_t w, uint16_t h) {
  *glfwWindow = glfwCreateWindow((int)w, (int)h, title.c_str(), NULL, NULL);

  if (!glfwWindow) {
    write_log("Failed to create GLFW Window");
    glfwTerminate();
  }

  glfwMakeContextCurrent(*glfwWindow);
}

static void GLFW_SetWindowCallbacks(GLFWwindow *glfwWindow,
                                    GLFWwindowclosefun glfwWindowCloseCb,
                                    GLFWcursorposfun glfwCursorPosCb,
                                    GLFWkeyfun glfwKeyCb) {
  glfwSetWindowCloseCallback(glfwWindow, glfwWindowCloseCb);
  glfwSetCursorPosCallback(glfwWindow, glfwCursorPosCb);
  glfwSetKeyCallback(glfwWindow, glfwKeyCb);
}

enum class GLFW_KEY_ACTIONS {};

static void GLFW_WindowLoop(GLFWwindow *glfwWindow) {
  /* while (!glfwWindowShouldClose(glfwWindow)) { */
  glfwSwapBuffers(glfwWindow);
  glfwPollEvents();
  /* } */
}

static void GLFW_Terminate() { glfwTerminate(); }

}; // namespace GLFW_Funcs
ENGINE_NAMESPACE_END
