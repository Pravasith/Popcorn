#include "RendererVk.h"
#include "BasicWorkflowVk.h"
#include "CommandPoolVk.h"
#include "DescriptorsVk.h"
#include "DeviceVk.h"
#include "FrameVk.h"
#include "FramebuffersVk.h"
#include "Material.h"
#include "MemoryAllocatorVk.h"
#include "Mesh.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Helpers.h"
#include "RenderWorkflowVk.h"
#include "SurfaceVk.h"
#include "SwapchainVk.h"
#include <cstdint>
#include <cstring>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

// Singleton members
DeviceVk *RendererVk::s_deviceVk = nullptr;
SurfaceVk *RendererVk::s_surfaceVk = nullptr;
SwapchainVk *RendererVk::s_swapchainVk = nullptr;
FramebuffersVk *RendererVk::s_framebuffersVk = nullptr;
CommandPoolVk *RendererVk::s_commandPoolVk = nullptr;
FrameVk *RendererVk::s_frameVk = nullptr;
MemoryAllocatorVk *RendererVk::s_memoryAllocatorVk = nullptr;

DescriptorSetLayoutsVk *RendererVk::s_descriptorSetLayoutsVk = nullptr;
// Other members

std::vector<RenderWorkflowVk *> RendererVk::s_renderWorkflows{};

//
// -------------------------------------------------------------------------
// --- PUBLIC METHODS ------------------------------------------------------

void RendererVk::DrawFrame(const Scene &scene) {
  BasicRenderWorkflowVk *basicRenderWorkflow =
      reinterpret_cast<BasicRenderWorkflowVk *>(
          s_renderWorkflows[(int)RenderWorkflowIndices::Basic]);

  s_frameVk->Draw(
      m_drawingCommandBuffers,

      // Pass final paint renderpass for swapchain recreation
      // TODO: Isolate this renderpass (move it outside basicWorkflow)
      basicRenderWorkflow->GetRenderPass(),

      // Update scene data lambda
      [&](const uint32_t currentFrame) {
        // TODO: Write a loop for render workflows instead
        basicRenderWorkflow->ProcessSceneUpdates(currentFrame);
      },

      // Record draw commands lambda
      [&](const uint32_t frameIndex, const uint32_t currentFrame,
          VkCommandBuffer &currentFrameCommandBuffer) {
        s_commandPoolVk->BeginCommandBuffer(currentFrameCommandBuffer);
        //
        // -----------------------------------------------------------------
        // --- RECORD ALL COMMAND BUFFERS HERE -----------------------------

        // TODO: Write a loop for render workflows instead
        basicRenderWorkflow->RecordRenderCommands(frameIndex, currentFrame,
                                                  currentFrameCommandBuffer);

        // --- RECORD ALL COMMAND BUFFERS HERE -----------------------------
        // -----------------------------------------------------------------
        //
        s_commandPoolVk->EndCommandBuffer(currentFrameCommandBuffer);
      });
};

bool RendererVk::OnFrameBufferResize(FrameBfrResizeEvent &) {
  s_frameVk->SetFrameBufferResized(true);
  return true;
};

void RendererVk::CreateBasicCommandBuffers() {
  auto *commandPoolVkStn = CommandPoolVk::Get();
  VkCommandBufferAllocateInfo allocInfo{};
  commandPoolVkStn->GetDefaultCommandBufferAllocInfo(allocInfo);
  allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;
  m_drawingCommandBuffers.resize(allocInfo.commandBufferCount);
  commandPoolVkStn->AllocCommandBuffers(allocInfo,
                                        m_drawingCommandBuffers.data());
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
  // TOOD: move to workflows
  s_descriptorSetLayoutsVk = DescriptorSetLayoutsVk::Get();
  s_memoryAllocatorVk = MemoryAllocatorVk::Get();

  s_swapchainVk->SetAppWindow(appWin);
};

RendererVk::~RendererVk() {
  VulkanCleanUp();
  PC_PRINT("DESTROYED", TagType::Destr, "RENDERER-VULKAN");
};

void RendererVk::VulkanCleanUp() {
  auto &instance = s_deviceVk->GetVkInstance();
  auto &device = s_deviceVk->GetDevice();

  // TOOD: move to workflows
  s_descriptorSetLayoutsVk->CleanUp();
  DescriptorSetLayoutsVk::Destroy();
  s_descriptorSetLayoutsVk = nullptr;

  for (auto *workflow : s_renderWorkflows) {
    workflow->CleanUp();
    delete workflow;
  }
  s_renderWorkflows.clear();

  s_memoryAllocatorVk->CleanUp();
  MemoryAllocatorVk::Destroy();
  s_memoryAllocatorVk = nullptr;

  s_frameVk->CleanUp();
  FrameVk::Destroy();
  s_frameVk = nullptr;

  s_commandPoolVk->CleanUp();
  CommandPoolVk::Destroy();
  s_commandPoolVk = nullptr;

  s_swapchainVk->CleanUp(device);
  SwapchainVk::Destroy();
  s_swapchainVk = nullptr;

  FramebuffersVk::Destroy();

  s_surfaceVk->CleanUp(instance);
  SurfaceVk::Destroy();
  s_surfaceVk = nullptr;

  s_deviceVk->CleanUp();
  DeviceVk::Destroy();
  s_deviceVk = nullptr;
};

//
// ---------------------------------------------------------------------------
// --- RENDER WORKFLOWS ------------------------------------------------------
void RendererVk::CreateRenderWorkflows() {
  BasicRenderWorkflowVk *basicRendererWorkflow = new BasicRenderWorkflowVk;
  s_renderWorkflows.push_back(basicRendererWorkflow);

  //
  // CREATE WORKFLOW RESOURCES -----------------------------------------------
  PC_WARN("Expensive initialization operation: Creating workflow Vulkan "
          "resources! Should only be done once per workflow object init.")
  for (auto &renderWorkflow : s_renderWorkflows) {
    renderWorkflow->CreateRenderPass();
    renderWorkflow->CreateFramebuffers();
  }
};

// Sort materials, allocate descriptor sets, vk buffers, index buffers &
// create pipelines
void RendererVk::CreateResources() {
  // Create VMA Allocator
  s_memoryAllocatorVk->CreateVMAAllocator();

  for (auto &renderWorkflow : s_renderWorkflows) {
    // Loops through all meshes & creates a contiguous Vulkan buffer memory for
    // each workflow -- each workflow has one VkBuffer & one VkDeviceMemory each
    renderWorkflow->AllocateVkVertexBuffers();
    renderWorkflow->AllocateVkIndexBuffers();
    renderWorkflow->AllocateVkUniformBuffers();
    renderWorkflow->CreateDescriptorSetLayouts();
    renderWorkflow->CreateDescriptorPool();
    renderWorkflow->CreateDescriptorSets();
    renderWorkflow->CreatePipelines();
  }
};

void RendererVk::AddMeshToWorkflow(Mesh *mesh) {
  BasicRenderWorkflowVk *basicRenderWorkflow =
      reinterpret_cast<BasicRenderWorkflowVk *>(
          s_renderWorkflows[(int)RenderWorkflowIndices::Basic]);
  basicRenderWorkflow->AddMeshToWorkflow(mesh);
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
  s_swapchainVk->CreateSwapchain();
  s_swapchainVk->CreateImageViews(device);

  //
  // RENDER READY ------------------------------------------------------------
  s_commandPoolVk->CreateCommandPool();
  s_frameVk->CreateRenderSyncObjects();
};

void RendererVk::ProcessScenes() {
  for (auto &scene : m_sceneLibrary.GetScenes()) {
    for (auto &node : scene->GetNodes()) {
      if (node->GetType() == GameObjectTypes::Mesh) {
        auto *meshPtr = (Mesh *)node;
        AddMeshToWorkflow(meshPtr);
      };
    };
  };
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
