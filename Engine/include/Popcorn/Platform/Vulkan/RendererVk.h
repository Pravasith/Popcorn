#pragma once

#include "CommandPoolVk.h"
#include "DeviceVk.h"
#include "FramebuffersVk.h"
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
  virtual void PrepareMaterialForRender(Material *materialPtr) override;

  // Sets up devices, configure swapchains, creates depth buffers
  // also allocates command pools
  void VulkanInit();
  void CreateRenderWorkflows();

  void VulkanDestroy();

  static RenderWorkflowVk *GetRenderWorkflow(const RenderWorkflowIndices index);
  void CreateBasicCommandBuffer();

private:
  void RenderScene() {

    // --- RENDER COLORS ---------------------------------------------------
    // BasicWorkflow::Render(Scene);
    // // Implementation inside BasicWorkflow::Render
    // auto regMaterials = Scene.GetRegisteredMaterials()
    // for (auto &mat : regMaterials) {
    //      for (auto &mesh : mat.GetMeshes()) {
    //         // Renderpass
    //         BasicPipelineVk.Get(mat.data, m_renderPass); // mat.data like
    //         uniforms
    //      };
    // };

    // --- RENDER SHADOWS --------------------------------------------------
    // ShadowsWorkflow::Render(Scene);
    // // Implementation inside ShadowsWorkflow::Render
    // auto regMaterials = Scene.GetRegisteredMaterials()
    // for (auto &mat : regMaterials) {
    //      for (auto &mesh : mat.GetMeshes()) {
    //         // Renderpass
    //         ShadowPipelineVk.Get(mat.data, m_renderPass); // mat.data like
    //         uniforms
    //      };
    // };
  };

  //
  // -----------------------------------------------------------------------
  // --- TODO: Potential methods -------------------------------------------
  void RecordCmdBuffer(void *renderPass, void *pipeline) {};
  void SubmitCmdBuffer(void *cmdBuffer) {};

private:
  static DeviceVk *s_deviceVk;
  static SurfaceVk *s_surfaceVk;
  static SwapchainVk *s_swapchainVk;
  static FramebuffersVk *s_framebuffersVk;
  static CommandPoolVk *s_commandPoolVk;

  static std::vector<RenderWorkflowVk *> s_renderWorkflows;

  VkCommandBuffer m_renderingCommandBuffer = VK_NULL_HANDLE;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
