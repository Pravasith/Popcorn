#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN

class WinSurfaceVk {
  friend class RendererVk;

  WinSurfaceVk(const VkInstance &vkInst) : m_vkInst(vkInst) {
    PC_PRINT("CREATED", TagType::Constr, "VK-WIN-SURFACE-VK");
  };

  ~WinSurfaceVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "VK-WIN-SURFACE-VK");
  };

  [[nodiscard]] inline const VkSurfaceKHR &GetSurface() const {
    return m_surface;
  };

  void CreateSurface(void *osWindow);
  void CleanUp();

private:
  const VkInstance &m_vkInst;
  VkSurfaceKHR m_surface;
};

ENGINE_NAMESPACE_END
