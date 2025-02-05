#include "SurfaceVk.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN
void SurfaceVk::CreateWindowSurface(const VkInstance &instance,
                                    GLFWwindow *window) {
  if (glfwCreateWindowSurface(instance, window, nullptr, &m_surface) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create window surface!");
  }
};

void SurfaceVk::CleanUp(const VkInstance &instance) {
  vkDestroySurfaceKHR(instance, m_surface, nullptr);
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
