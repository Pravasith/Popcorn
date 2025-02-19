#pragma once

#include "DeviceVk.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Window.h"
#include "Popcorn/Events/WindowEvent.h"
#include "RenderWorkflowVk.h"
#include "Renderer.h"
#include "SurfaceVk.h"
#include "SwapchainVk.h"
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class RendererVk : public Renderer {
public:
  RendererVk(const Window &appWin);
  virtual ~RendererVk() override;

  // Can potentially take "void *frame" as a param
  virtual void DrawFrame(const Scene &scene) const override;
  virtual bool OnFrameBfrResize(FrameBfrResizeEvent &) override;
  virtual void CreateRenderWorkflows() override;

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

private:
  static DeviceVk *s_deviceVk;
  static SurfaceVk *s_surfaceVk;
  static SwapchainVk *s_swapchainVk;

  static std::vector<RenderWorkflowVk> s_renderWorkflows;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
