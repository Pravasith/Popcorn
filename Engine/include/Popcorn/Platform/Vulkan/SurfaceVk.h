#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class SurfaceVk {
public:
  SurfaceVk() { PC_PRINT("CREATED", TagType::Constr, "SurfaceVk.h"); };
  ~SurfaceVk() { PC_PRINT("DESTROYED", TagType::Destr, "SurfaceVk.h"); };

  [[nodiscard]] inline const VkSurfaceKHR &GetSurface() {
    if (m_surface == VK_NULL_HANDLE) {
      PC_WARN("Attempt to access VkSurface before it's creation")
    };
    return m_surface;
  };

  inline void CreateWindowSurface(const VkInstance &instance,
                                  GLFWwindow *window) {
    if (glfwCreateWindowSurface(instance, window, nullptr, &m_surface) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to create window surface!");
    }
  };

private:
  VkSurfaceKHR m_surface = VK_NULL_HANDLE;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
