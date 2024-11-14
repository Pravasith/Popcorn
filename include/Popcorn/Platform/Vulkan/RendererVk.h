#pragma once

#include "Global_Macros.h"
#include "PhysDeviceVk.h"
#include "Renderer.h"
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "ValidationLyrsVk.h"

ENGINE_NAMESPACE_BEGIN
class RendererVk : public Renderer {
public:
  RendererVk();
  ~RendererVk();

private:
  void InitVulkan();
  void CleanUp();

  void CreateInstance();

  std::vector<const char *> GetRequiredExtensions();
  virtual void OnUpdate() const override;

private:
  // MEMBERS
  VkInstance m_vkInstance;
#ifdef NDEBUG
  static constexpr bool s_enableValidationLayers = false;
#else
  static constexpr bool s_enableValidationLayers = true;
#endif

  ValidationLyrsVk m_ValLyrsVk;
  PhysDeviceVk m_PhysDevVk;
};

ENGINE_NAMESPACE_END
