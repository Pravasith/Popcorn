#pragma once

#include "Global_Macros.h"
#include "LogiDeviceVk.h"
#include "PhysDeviceVk.h"
#include "Renderer.h"
#include "WinSurfaceVk.h"
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "ValidationLyrsVk.h"

ENGINE_NAMESPACE_BEGIN
class RendererVk : public Renderer {
public:
  RendererVk();
  ~RendererVk();

  using QueueFamilyIndicesVk = PhysDeviceVk::QueueFamilyIndices;

  constexpr inline static bool IsValLyrsEnabled() {
    return s_enableValidationLayers;
  };

private:
  void InitVulkan();
  void CleanUp();

  void CreateInstance();

  std::vector<const char *> GetRequiredExtensions();
  virtual void OnUpdate() const override;

private:
  [[nodiscard]] inline const VkSurfaceKHR &GetSurface() const {
    return m_WinSrfcVk.GetSurface();
  };

  VkInstance m_vkInstance;
#ifdef NDEBUG
  static constexpr bool s_enableValidationLayers = false;
#else
  static constexpr bool s_enableValidationLayers = true;
#endif

  ValidationLyrsVk m_ValLyrsVk;
  PhysDeviceVk m_PhysDevVk;
  LogiDeviceVk m_LogiDevVk;
  WinSurfaceVk m_WinSrfcVk;

  const QueueFamilyIndicesVk &m_qFamIndices;
};

ENGINE_NAMESPACE_END
