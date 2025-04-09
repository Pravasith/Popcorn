#pragma once

#include "ContextVk.h"
#include "GameObject.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Window.h"
#include "Popcorn/Events/WindowEvent.h"
#include "RenderFlows/RenderFlowVk.h"
#include "Renderer.h"
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

  virtual void CreateRenderFlows() override;
  virtual void AssignSceneObjectsToRenderFlows() override;
  virtual void CreateRenderFlowResources() override;

  virtual void ProcessGameObjectNode(GameObject *node) override;

  virtual void DrawFrame(const Scene &scene) override;

  virtual bool OnFrameBufferResize(FrameBfrResizeEvent &) override;

  // Sets up devices, configure swapchains, creates depth buffers
  // also allocates command pools
  void CreateVulkanContext();
  void DestroyVulkanContext();

  static RenderFlowVk *GetRenderFlow(const RenderFlows index);
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
  static ContextVk *s_vulkanContext;
  static std::vector<RenderFlowVk *> s_renderFlows;
  std::vector<VkCommandBuffer> m_drawingCommandBuffers;

  std::vector<Submesh<MaterialTypes::BasicMat> *> m_basicSubmeshes;
  std::vector<Submesh<MaterialTypes::PbrMat> *> m_pbrSubmeshes;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
