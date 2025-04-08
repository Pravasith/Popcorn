#include "RendererVk.h"
#include "CommandPoolVk.h"
#include "ContextVk.h"
#include "Material.h"
#include "Mesh.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Helpers.h"
#include "RenderFlows/BasicRenderFlowVk.h"
#include <cstring>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

ContextVk *RendererVk::s_vulkanContext = nullptr;
std::vector<RenderFlowVk *> RendererVk::s_renderWorkflows{};

//
// -------------------------------------------------------------------------
// --- PUBLIC METHODS ------------------------------------------------------

void RendererVk::DrawFrame(const Scene &scene) {
  BasicRenderFlowVk *basicRenderFlow = reinterpret_cast<BasicRenderFlowVk *>(
      s_renderWorkflows[(int)RenderFlows::Basic]);

  ContextVk::Frame()->Draw(
      m_drawingCommandBuffers,

      // Pass final paint renderpass for swapchain recreation
      // TODO: Isolate this renderpass (move it outside basicWorkflow)
      basicRenderFlow->GetRenderPass(),

      // Update scene data lambda
      [&](const uint32_t currentFrame) {
        // TODO: Write a loop for render workflows instead
        basicRenderFlow->ProcessSceneUpdates(currentFrame);
      },

      // Record draw commands lambda
      [&](const uint32_t frameIndex, const uint32_t currentFrame,
          VkCommandBuffer &currentFrameCommandBuffer) {
        ContextVk::CommandPool()->BeginCommandBuffer(currentFrameCommandBuffer);
        //
        // -----------------------------------------------------------------
        // --- RECORD ALL COMMAND BUFFERS HERE -----------------------------

        // TODO: Write a loop for render workflows instead
        basicRenderFlow->RecordRenderCommands(frameIndex, currentFrame,
                                              currentFrameCommandBuffer);

        // --- RECORD ALL COMMAND BUFFERS HERE -----------------------------
        // -----------------------------------------------------------------
        //
        ContextVk::CommandPool()->EndCommandBuffer(currentFrameCommandBuffer);
      });
};

bool RendererVk::OnFrameBufferResize(FrameBfrResizeEvent &) {
  ContextVk::Frame()->SetFrameBufferResized(true);
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
  CreateVulkanContext();
};

RendererVk::~RendererVk() {
  // Renderflow clean ups
  for (auto *workflow : s_renderWorkflows) {
    workflow->CleanUp();
    delete workflow;
  }
  s_renderWorkflows.clear();

  DestroyVulkanContext();
  PC_PRINT("DESTROYED", TagType::Destr, "RENDERER-VULKAN");
};

void RendererVk::CreateVulkanContext() {
  s_vulkanContext = ContextVk::Get();
  s_vulkanContext->VulkanInit(m_AppWin);
};

void RendererVk::DestroyVulkanContext() {
  s_vulkanContext->VulkanCleanUp();
  ContextVk::Destroy();
  s_vulkanContext = nullptr;
};

//
// ---------------------------------------------------------------------------
// --- RENDER WORKFLOWS ------------------------------------------------------
void RendererVk::CreateRenderFlows() {
  BasicRenderFlowVk *basicRendererWorkflow = new BasicRenderFlowVk;
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
void RendererVk::CreateRenderFlowResources() {
  // Create VMA Allocator
  ContextVk::MemoryAllocator()->CreateVMAAllocator();

  for (auto &renderWorkflow : s_renderWorkflows) {
    // Loops through all meshes & creates a contiguous Vulkan buffer memory for
    // each workflow -- each workflow has one VkBuffer & one VkDeviceMemory each
    renderWorkflow->AllocateVkVertexBuffers();    // VMA - Extract from meshes
    renderWorkflow->AllocateVkIndexBuffers();     // VMA - Extract from meshes
    renderWorkflow->AllocateVkUniformBuffers();   // VMA - Extract from -
                                                  // 1. Camera
                                                  // 2. Mesh
                                                  // 3. Mesh material
    renderWorkflow->CreateDescriptorSetLayouts(); // Done
    renderWorkflow->CreateDescriptorPool();       // Move to factory
    renderWorkflow->CreateDescriptorSets();       // Move to factory
    renderWorkflow->CreatePipelines();            // ALMOST DONE
  }
};

void RendererVk::AssignSceneObjectsToRenderFlows() {
  // Attach Meshes to Workflows (ptrs)
  // Segregate to submesh types
  // Create Vulkan resources:
  //      - Vertex buffer memory
  //      - Index buffer memory
  //      - Descriptor sets (for every frame)
  //            - Camera UBO
  //            - Per mesh UBO (world matrix)
  //            - Per material UBO (tbd)
  for (auto &scene : m_sceneLibrary.GetScenes()) {
    for (auto &node : scene->GetNodes()) {
      // Recurse
      // ProcessGameObjectNode(node);
    };
  };

  // BasicRenderFlowVk *basicRenderFlow = reinterpret_cast<BasicRenderFlowVk *>(
  //     s_renderWorkflows[(int)RenderFlows::Basic]);
  // basicRenderFlow->AddMeshToWorkflow(mesh);
};

RenderFlowVk *RendererVk::GetRenderFlow(const RenderFlows index) {
  return s_renderWorkflows[(int)index];
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
