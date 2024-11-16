#pragma once

#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"
#include <stdexcept>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN

class WinSurfaceVk {
  friend class RendererVk;

  WinSurfaceVk(const VkInstance &vkInst, GLFWwindow *osWindow)
      : m_vkInst(vkInst), m_osWindow(osWindow) {
    PC_PRINT("CREATED", TagType::Constr, "VK-WIN-SURFACE-VK");
  };

  ~WinSurfaceVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "VK-WIN-SURFACE-VK");
  };

  void CreateSurface() {
    if (glfwCreateWindowSurface(m_vkInst, m_osWindow, nullptr, &m_surface) !=
        VK_SUCCESS) {
      throw std::runtime_error("FAILED TO CREATE WINDOW SURFACE!");
    }
  }

  const VkInstance &m_vkInst;

  GLFWwindow *m_osWindow;
  VkSurfaceKHR m_surface;
};

ENGINE_NAMESPACE_END
