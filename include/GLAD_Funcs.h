#pragma once

#include "Utilities.h"
#include <glad/glad.h>
// A LINE BREAK FOR FORMATTING

namespace GLAD_Funcs {

typedef void (*GLFW_Glproc_Type)(void);
typedef GLFW_Glproc_Type (*GLFW_Get_Proc_Address)(const char *);

static void Init(GLFW_Get_Proc_Address get_proc_address_cb) {
  if (!gladLoadGLLoader((GLADloadproc)get_proc_address_cb)) {
    write_log("Failed to initialize GLAD");
  }
}

static void Set_Viewport(GLint x, GLint y, GLsizei width, GLsizei height) {
  glViewport(x, y, width, height);
}

}; // namespace GLAD_Funcs
