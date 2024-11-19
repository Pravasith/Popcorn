#include "WinSurfaceVk.h"
#include "Global_Macros.h"

ENGINE_NAMESPACE_BEGIN
void WinSurfaceVk::CreateSurface() {
  if (glfwCreateWindowSurface(m_vkInst, m_osWindow, nullptr, &m_surface) !=
      VK_SUCCESS) {
    throw std::runtime_error("FAILED TO CREATE WINDOW SURFACE!");
  }
}

void WinSurfaceVk::CleanUp() {
  vkDestroySurfaceKHR(m_vkInst, m_surface, nullptr);
}

ENGINE_NAMESPACE_END
