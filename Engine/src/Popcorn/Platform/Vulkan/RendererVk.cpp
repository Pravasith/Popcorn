#include "RendererVk.h"
#include "BasicWorkflowVk.h"
#include "CommandPoolVk.h"
#include "DeviceVk.h"
#include "FramebuffersVk.h"
#include "Material.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Helpers.h"
#include "RenderWorkflowVk.h"
#include "SurfaceVk.h"
#include "SwapchainVk.h"
#include <cstring>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

// Singleton members
DeviceVk *RendererVk::s_deviceVk = DeviceVk::Get();
SurfaceVk *RendererVk::s_surfaceVk = SurfaceVk::Get();
SwapchainVk *RendererVk::s_swapchainVk = SwapchainVk::Get();
FramebuffersVk *RendererVk::s_framebuffersVk = FramebuffersVk::Get();
CommandPoolVk *RendererVk::s_commandPoolVk = CommandPoolVk::Get();
// Other members
std::vector<RenderWorkflowVk *> RendererVk::s_renderWorkflows{};

//
// -------------------------------------------------------------------------
// --- PUBLIC METHODS ------------------------------------------------------

void RendererVk::DrawFrame(const Scene &scene) const {};
bool RendererVk::OnFrameBfrResize(FrameBfrResizeEvent &) { return true; };

void RendererVk::PrepareMaterialForRender(Material *materialPtr) {
  switch (materialPtr->GetMaterialType()) {
  case MaterialTypes::BasicMat:
    // Creates Vulkan Pipelines necessary for the basic materials
    {
      PC_WARN(s_renderWorkflows.size());

      auto *basicRenderWorkflow =
          s_renderWorkflows[(int)RenderWorkflowIndices::Basic];
      basicRenderWorkflow->CreateWorkflowResources(materialPtr);
    }
    break;
  case MaterialTypes::PbrMat:
    break;
  }
};

//
// -------------------------------------------------------------------------
// --- PRIVATE METHODS -----------------------------------------------------

RendererVk::RendererVk(const Window &appWin) : Renderer(appWin) {
  PC_PRINT("CREATED", TagType::Constr, "RENDERER-VK");
};

RendererVk::~RendererVk() {
  // Destroy render workflows
  for (auto *workflow : s_renderWorkflows) {
    delete workflow;
  }
  s_renderWorkflows.clear();

  VulkanDestroy();

  FramebuffersVk::Destroy();
  SwapchainVk::Destroy();
  SurfaceVk::Destroy();
  DeviceVk::Destroy();

  PC_PRINT("DESTROYED", TagType::Destr, "RENDERER-VULKAN");
};

//
// ---------------------------------------------------------------------------
// --- RENDER WORKFLOWS ------------------------------------------------------
void RendererVk::CreateRenderWorkflows() {
  BasicRenderWorkflowVk *basicRendererWorkflow = new BasicRenderWorkflowVk;
  s_renderWorkflows.push_back(basicRendererWorkflow);
};

RenderWorkflowVk *
RendererVk::GetRenderWorkflow(const RenderWorkflowIndices index) {
  return s_renderWorkflows[(int)index];
};

void RendererVk::VulkanInit() {
  GLFWwindow *osWindow = static_cast<GLFWwindow *>(m_AppWin.GetOSWindow());
  //
  // CREATE INSTANCE, SET UP DEBUGGING LAYERS --------------------------------
  s_deviceVk->CreateInstance({"Vulkan App", 1, 0, 0});
  s_deviceVk->SetupDebugMessenger();

  const auto &instance = s_deviceVk->GetVkInstance();

  //
  // CREATE WINDOW SURFACE ---------------------------------------------------
  s_surfaceVk->CreateWindowSurface(instance, osWindow);

  const auto &surface = s_surfaceVk->GetSurface();

  //
  // CREATE PHYSICAL & LOGICAL DEVICE ----------------------------------------
  s_deviceVk->PickPhysicalDevice(surface);
  s_deviceVk->CreateLogicalDevice(surface);

  const auto &device = s_deviceVk->GetDevice();
  const auto &swapchainSupportDetails =
      s_deviceVk->GetSwapchainSupportDetails(surface);
  const auto &queueFamilyIndices = s_deviceVk->GetQueueFamilyIndices(surface);

  //
  // CREATE SWAPCHAIN --------------------------------------------------------
  s_swapchainVk->CreateSwapchain(device, swapchainSupportDetails, osWindow,
                                 surface, queueFamilyIndices);
  s_swapchainVk->CreateImageViews(device);
};

void RendererVk::VulkanDestroy() {
  const auto &instance = s_deviceVk->GetVkInstance();
  const auto &device = s_deviceVk->GetDevice();

  s_swapchainVk->CleanUp(device);
  s_surfaceVk->CleanUp(instance);
  s_deviceVk->CleanUp();
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
