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
  // virtual RenderWorkflowVk*

  // Sets up devices, configure swapchains, creates depth buffers
  // also allocates command pools
  void VulkanInit();
  void CreateRenderWorkflows();

  // Temp functions
  void CreateTrianglePipeline();
  void CreateTriangleRenderPass();

  void VulkanDestroy();

  static RenderWorkflowVk *GetRenderWorkflow(const RenderWorkflowIndices index);

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

  static std::vector<RenderWorkflowVk *> s_renderWorkflows;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
