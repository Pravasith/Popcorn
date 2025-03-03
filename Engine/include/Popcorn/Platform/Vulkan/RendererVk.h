#pragma once

#include "CommandPoolVk.h"
#include "DeviceVk.h"
#include "FrameVk.h"
#include "FramebuffersVk.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Window.h"
#include "Popcorn/Events/WindowEvent.h"
#include "RenderWorkflowVk.h"
#include "Renderer.h"
#include "SurfaceVk.h"
#include "SwapchainVk.h"
#include <cstdint>
#include <vector>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class RendererVk : public Renderer {
public:
  RendererVk(const Window &appWin);
  virtual ~RendererVk() override;

  virtual void DrawFrame(const Scene &scene) override;
  virtual bool OnFrameBufferResize(FrameBfrResizeEvent &) override;
  virtual void CreateMaterialPipeline(Material *materialPtr) override;
  virtual void AddMeshToWorkflow(Mesh *meshPtr) override;

  // Sets up devices, configure swapchains, creates depth buffers
  // also allocates command pools
  void VulkanInit();
  void CreateRenderWorkflows();

  void VulkanCleanUp();

  static RenderWorkflowVk *GetRenderWorkflow(const RenderWorkflowIndices index);
  void CreateBasicCommandBuffers();

private:
  //
  // -----------------------------------------------------------------------
  // --- TODO: Potential methods -------------------------------------------
  void RecordCmdBuffer(void *renderPass, void *pipeline) {};
  void SubmitCmdBuffer(void *cmdBuffer) {};

public:
  static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;

private:
  static DeviceVk *s_deviceVk;
  static SurfaceVk *s_surfaceVk;
  static SwapchainVk *s_swapchainVk;
  static FramebuffersVk *s_framebuffersVk;
  static CommandPoolVk *s_commandPoolVk;
  static FrameVk *s_frameVk;

  static std::vector<RenderWorkflowVk *> s_renderWorkflows;

  std::vector<VkCommandBuffer> m_drawingCommandBuffers;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
