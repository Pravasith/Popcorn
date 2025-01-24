#include "WinSurfaceVk.h"
#include "GlobalMacros.h"

ENGINE_NAMESPACE_BEGIN
void WinSurfaceVk::CreateSurface(void *osWindow) {
  if (glfwCreateWindowSurface(m_vkInst, static_cast<GLFWwindow *>(osWindow),
                              nullptr, &m_surface) != VK_SUCCESS) {
    throw std::runtime_error("FAILED TO CREATE WINDOW SURFACE!");
  }
}

void WinSurfaceVk::CleanUp() {
  vkDestroySurfaceKHR(m_vkInst, m_surface, nullptr);
}

ENGINE_NAMESPACE_END
