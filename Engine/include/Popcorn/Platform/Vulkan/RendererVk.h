#pragma once

#include "ContextVk.h"
#include "GameObject.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Window.h"
#include "Popcorn/Events/WindowEvent.h"
#include "RenderFlowVk.h"
#include "Renderer.h"
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

  // Before GameLoop
  virtual void CreateRenderFlows() override;
  virtual void DestroyRenderFlows() override;
  virtual void PrepareRenderFlows() override;
  virtual void AssignSceneObjectsToRenderFlows() override;
  virtual void CreateRenderFlowResources() override;

  virtual void ProcessGameObjectNode(GameObject *node) override;

  virtual void DrawFrame(const Scene &scene) override;

  virtual bool OnFrameBufferResize(FrameBfrResizeEvent &) override;

  // Sets up devices, configure swapchains, creates depth buffers
  // also allocates command pools
  void CreateVulkanContext();
  void DestroyVulkanContext();

private:
  static ContextVk *s_vulkanContext;
  static std::vector<RenderFlowVk *> s_renderFlows;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
