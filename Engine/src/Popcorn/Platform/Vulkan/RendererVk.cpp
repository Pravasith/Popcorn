#include "RendererVk.h"
#include "BasicWorkflowVk.h"
#include "CommandPoolVk.h"
#include "DeviceVk.h"
#include "FrameVk.h"
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
DeviceVk *RendererVk::s_deviceVk = nullptr;
SurfaceVk *RendererVk::s_surfaceVk = nullptr;
SwapchainVk *RendererVk::s_swapchainVk = nullptr;
FramebuffersVk *RendererVk::s_framebuffersVk = nullptr;
CommandPoolVk *RendererVk::s_commandPoolVk = nullptr;
FrameVk *RendererVk::s_frameVk = nullptr;
// Other members
std::vector<RenderWorkflowVk *> RendererVk::s_renderWorkflows{};

//
// -------------------------------------------------------------------------
// --- PUBLIC METHODS ------------------------------------------------------

void RendererVk::DrawFrame(const Scene &scene) {
  BasicRenderWorkflowVk *basicRenderWorkflow =
      reinterpret_cast<BasicRenderWorkflowVk *>(
          s_renderWorkflows[(int)RenderWorkflowIndices::Basic]);

  s_frameVk->Draw(m_drawingCommandBuffer,
                  // Record commands lambda
                  [&](const uint32_t frameIndex) {
                    s_commandPoolVk->BeginCommandBuffer(m_drawingCommandBuffer);
                    // TODO: Write a loop for render workflows instead
                    // {
                    basicRenderWorkflow->RecordRenderCommands(
                        scene, m_drawingCommandBuffer, frameIndex);
                    // }
                    s_commandPoolVk->EndCommandBuffer(m_drawingCommandBuffer);
                  });
};

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

void RendererVk::CreateBasicCommandBuffer() {
  auto *commandPoolVkStn = CommandPoolVk::Get();
  VkCommandBufferAllocateInfo allocInfo{};

  commandPoolVkStn->GetDefaultCommandBufferAllocInfo(allocInfo);
  commandPoolVkStn->AllocCommandBuffer(allocInfo, m_drawingCommandBuffer);
}

//
// -------------------------------------------------------------------------
// --- PRIVATE METHODS -----------------------------------------------------

RendererVk::RendererVk(const Window &appWin) : Renderer(appWin) {
  PC_PRINT("CREATED", TagType::Constr, "RENDERER-VK");

  s_deviceVk = DeviceVk::Get();
  s_surfaceVk = SurfaceVk::Get();
  s_swapchainVk = SwapchainVk::Get();
  s_framebuffersVk = FramebuffersVk::Get();
  s_commandPoolVk = CommandPoolVk::Get();
  s_frameVk = FrameVk::Get();
};

RendererVk::~RendererVk() {
  VulkanCleanUp();
  PC_PRINT("DESTROYED", TagType::Destr, "RENDERER-VULKAN");
};

void RendererVk::VulkanCleanUp() {
  auto &instance = s_deviceVk->GetVkInstance();
  auto &device = s_deviceVk->GetDevice();

  for (auto *workflow : s_renderWorkflows) {
    workflow->CleanUp();
    delete workflow;
  }

  s_frameVk->CleanUp();
  FrameVk::Destroy();

  s_commandPoolVk->CleanUp();
  CommandPoolVk::Destroy();

  s_renderWorkflows.clear();
  FramebuffersVk::Destroy();

  s_swapchainVk->CleanUp(device);
  SwapchainVk::Destroy();

  s_surfaceVk->CleanUp(instance);
  SurfaceVk::Destroy();

  s_deviceVk->CleanUp();
  DeviceVk::Destroy();
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

  //
  // RENDER READY ------------------------------------------------------------
  s_commandPoolVk->CreateCommandPool();
  s_frameVk->CreateRenderSyncObjects();
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
