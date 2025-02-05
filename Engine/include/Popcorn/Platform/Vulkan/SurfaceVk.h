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
    PC_VK_NULL_CHECK(m_surface)
    return m_surface;
  };

  void CreateWindowSurface(const VkInstance &, GLFWwindow *);
  void CleanUp(const VkInstance &);

private:
  VkSurfaceKHR m_surface = VK_NULL_HANDLE;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
