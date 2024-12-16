#pragma once

#include "CmdPoolVk.h"
#include "CommonVk.h"
#include "GfxPipelineVk.h"
#include "Global_Macros.h"
#include "LogiDeviceVk.h"
#include "PhysDeviceVk.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Events/Event.h"
#include "PresentVk.h"
#include "Renderer.h"
#include "SwapChainVk.h"
#include "ValidationLyrsVk.h"
#include "WinSurfaceVk.h"
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
class RendererVk : public Renderer<RendererType::Vulkan> {
public:
  RendererVk(const Window &appWin);
  ~RendererVk();

  constexpr inline static bool IsValLyrsEnabled() {
    return s_enableValidationLayers;
  };

  void OnUpdate() override;
  void OnEvent(Event &e) override;

private:
  void InitVulkan();
  void CleanUp();

  void CreateInstance();

  std::vector<const char *> GetRequiredExtensions();

private:
  [[nodiscard]] inline const VkSurfaceKHR &GetSurface() const {
    if (!m_WinSrfcVk.GetSurface()) {
      PC_WARN("SURFACE NOT AVAILABLE");
    };
    return m_WinSrfcVk.GetSurface();
  };

  [[nodiscard]] inline const VkPhysicalDevice &GetPhysDevice() const {
    if (!m_PhysDevVk.GetPhysDevice()) {
      PC_WARN("PHYS DEV NOT AVAILABLE");
    };
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
  GfxPipelineVk m_GfxPlineVk;
  SwapChainVk m_SwpChnVk;
  CmdPoolVk m_CmdPoolVk;
  const QueueFamilyIndices &m_qFamIndices;

  PresentVk m_PresentVk;
};

ENGINE_NAMESPACE_END
