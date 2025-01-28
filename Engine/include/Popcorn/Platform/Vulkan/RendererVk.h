#pragma once

#include "CmdPoolVk.h"
#include "CommonVk.h"
#include "GfxPipelineVk.h"
#include "GlobalMacros.h"
#include "LogiDeviceVk.h"
#include "PhysDeviceVk.h"
#include "Popcorn/Core/Base.h"
#include "PresentVk.h"
#include "Renderer.h"
#include "SwapChainVk.h"
#include "ValidationLyrsVk.h"
#include "VertexBufferVk.h"
#include "WinSurfaceVk.h"
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class RendererVk : public Renderer {
public:
  RendererVk(const Window &appWin);
  ~RendererVk();

  constexpr inline static bool AreValLayersEnabled() {
    return s_enableValidationLayers;
  };

  virtual void DrawFrame() override;
  virtual bool OnFrameBfrResize(FrameBfrResizeEvent &) override;

  inline void BindVertexBuffer(VertexBufferVk &vertexBufferVk) {
    m_vertexBufferVk = std::move(vertexBufferVk);
  };

  void InitVulkan();

private:
  void CleanUp();

  void CreateInstance();

  std::vector<const char *> GetRequiredExtensions();

private:
  [[nodiscard]] inline const VkSurfaceKHR &GetSurface() const {
    if (!m_WinSurfaceVk.GetSurface()) {
      PC_WARN("SURFACE NOT AVAILABLE");
    };
    return m_WinSurfaceVk.GetSurface();
  };

  [[nodiscard]] inline const VkPhysicalDevice &GetPhysDevice() const {
    if (!m_PhysDeviceVk.GetPhysDevice()) {
      PC_WARN("PHYS DEV NOT AVAILABLE");
    };
    return m_PhysDeviceVk.GetPhysDevice();
  };

  VkInstance m_vkInstance;

#ifdef NDEBUG
  static constexpr bool s_enableValidationLayers = false;
#else
  static constexpr bool s_enableValidationLayers = true;
#endif

  ValidationLyrsVk m_ValLayersVk;
  WinSurfaceVk m_WinSurfaceVk;
  PhysDeviceVk m_PhysDeviceVk;
  LogiDeviceVk m_LogiDeviceVk;
  GfxPipelineVk m_GfxPipelineVk;
  SwapChainVk m_SwapChainVk;
  CmdPoolVk m_CmdPoolVk;
  const QueueFamilyIndices &m_queueFamilyIndices;
  PresentVk m_PresentVk;

  VertexBufferVk m_vertexBufferVk;
};
GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
