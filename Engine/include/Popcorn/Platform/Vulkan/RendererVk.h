#pragma once

#include "DeviceVk.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Window.h"
#include "Popcorn/Events/WindowEvent.h"
#include "Renderer.h"
#include "SurfaceVk.h"
#include "SwapchainVk.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class RendererVk : public Renderer {
public:
  RendererVk(const Window &appWin);
  ~RendererVk() override;

  // Sets up devices, configure swapchains, creates depth buffers
  // also allocates command pools
  void VulkanInit();

  // Temp functions
  void CreateTrianglePipeline();
  void CreateTriangleRenderPass();

  void VulkanDestroy();

  // STATIC UTILS BEGIN ----------------------------------------------
  static void CreateShaderModules() {};

  static void BindPipeline(void *pipeline, void *renderPass) {};

  static void BeginRenderPass(void *polyRenderPass) {};
  static void BeginSubRenderPass(void *polyRenderPass) {};

  static void EndRenderPass(void *polySubrenderPass) {};
  static void EndSubRenderPass(void *polySubrenderPass) {};
  // STATIC UTILS END ------------------------------------------------

  void RecordCmdBuffer(void *renderPass, void *pipeline) {};
  void SubmitCmdBuffer(void *cmdBuffer) {};

  // Can potentially take "void *frame" as a param
  virtual void DrawFrame(const Scene &scene) const override;
  virtual bool OnFrameBfrResize(FrameBfrResizeEvent &) override;

private:
  DeviceVk m_deviceVk;
  SurfaceVk m_surfaceVk;
  SwapchainVk m_swapchainVk;

  // std::vector<VkDescriptorPool> s_descriptorPools;
  // std::vector<VkRenderPass> s_renderPasses;

  // SWAPCHAIN IMAGES
  // uint32_t s_minImageCount;
  // uint32_t s_imageCount;
  // uint32_t s_subpass;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
