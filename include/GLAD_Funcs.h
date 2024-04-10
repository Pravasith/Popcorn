#pragma once

#include <glad/glad.h>
// A LINE BREAK FOR FORMATTING
#include "GLFW_Funcs.h"

namespace GLAD_Funcs {
static void Init() {
  if (!gladLoadGLLoader((GLADloadproc)GLFW_Funcs::Get_Proc_Address)) {
    write_log("Failed to initialize GLAD");
  }
}

static void Set_Viewport(GLint x, GLint y, GLsizei width, GLsizei height) {
  glViewport(x, y, width, height);
}

}; // namespace GLAD_Funcs
