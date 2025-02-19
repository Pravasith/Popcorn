#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class SurfaceVk {
public:
  static SurfaceVk *Get() {
    if (s_instance) {
      return s_instance;
    };

    s_instance = new SurfaceVk();
    return s_instance;
  };

  static void Destroy() {
    if (s_instance) {
      delete s_instance;
      s_instance = nullptr;
    } else {
      PC_WARN("Trying to destroy a non-existant instance of SurfaceVk")
    };
  };

  [[nodiscard]] inline const VkSurfaceKHR &GetSurface() {
    PC_VK_NULL_CHECK(m_surface)
    return m_surface;
  };

  void CreateWindowSurface(const VkInstance &, GLFWwindow *);
  void CleanUp(const VkInstance &);

private:
  SurfaceVk() { PC_PRINT("CREATED", TagType::Constr, "SurfaceVk.h"); };
  ~SurfaceVk() { PC_PRINT("DESTROYED", TagType::Destr, "SurfaceVk.h"); };

  VkSurfaceKHR m_surface = VK_NULL_HANDLE;

  static SurfaceVk *s_instance;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
