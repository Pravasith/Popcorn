#pragma once

#include "CmdPoolVk.h"
#include "CommonVk.h"
#include "GfxPipelineVk.h"
#include "Global_Macros.h"
#include "LogiDeviceVk.h"
#include "PhysDeviceVk.h"
#include "PresentVk.h"
#include "Renderer.h"
#include "SwapChainVk.h"
#include "WinSurfaceVk.h"
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "ValidationLyrsVk.h"

ENGINE_NAMESPACE_BEGIN
class RendererVk : public Renderer<RendererType::Vulkan> {
public:
  RendererVk(const Window &appWin);
  ~RendererVk();

  constexpr inline static bool IsValLyrsEnabled() {
    return s_enableValidationLayers;
  };

  void OnUpdate() override;

private:
  void InitVulkan();
  void CleanUp();

  void CreateInstance();

  std::vector<const char *> GetRequiredExtensions();

private:
  [[nodiscard]] inline const VkSurfaceKHR &GetSurface() const {
    return m_WinSrfcVk.GetSurface();
  };

  [[nodiscard]] inline const VkPhysicalDevice &GetPhysDevice() const {
    return m_PhysDevVk.GetPhysDevice();
  };

  VkInstance m_vkInstance;
#ifdef NDEBUG
  static constexpr bool s_enableValidationLayers = false;
#else
  static constexpr bool s_enableValidationLayers = true;
#endif

  ValidationLyrsVk m_ValLyrsVk;
  WinSurfaceVk m_WinSrfcVk;
  PhysDeviceVk m_PhysDevVk;
  LogiDeviceVk m_LogiDevVk;
  SwapChainVk m_SwpChnVk;
  GfxPipelineVk m_GfxPlineVk;
  CmdPoolVk m_CmdPoolVk;
  PresentVk m_PresentVk;

  const QueueFamilyIndices &m_qFamIndices;
};

ENGINE_NAMESPACE_END
