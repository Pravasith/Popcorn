#include "RendererVk.h"
#include "CommandPoolVk.h"
#include "ContextVk.h"
#include "GameObject.h"
#include "Material.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Helpers.h"
#include "RenderFlows/BasicRenderFlowVk.h"
#include "RenderFlows/CompositeRenderFlowVk.h"
#include "RenderFlows/GBufferRenderFlowVk.h"
#include "RenderFlows/LightingRenderFlowVk.h"
#include "RenderFlows/RenderFlowVk.h"
#include <cstring>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

ContextVk *RendererVk::s_vulkanContext = nullptr;
std::vector<RenderFlowVk *> RendererVk::s_renderFlows{};

//
// -------------------------------------------------------------------------
// --- PUBLIC METHODS ------------------------------------------------------

void RendererVk::DrawFrame(const Scene &scene) {
  BasicRenderFlowVk *basicRenderFlow = reinterpret_cast<BasicRenderFlowVk *>(
      s_renderFlows[(int)RenderFlows::Basic]);

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

void RendererVk::CreateRenderingCommandBuffers() {
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
  for (auto *workflow : s_renderFlows) {
    workflow->CleanUp();
    delete workflow;
  }
  s_renderFlows.clear();

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
  s_renderFlows.emplace_back(new GBufferRenderFlowVk());
  s_renderFlows.emplace_back(new LightingRenderFlowVk());
  s_renderFlows.emplace_back(new CompositeRenderFlowVk());

  for (auto &renderFlow : s_renderFlows) {
    PC_WARN("Preparing renderflow...")
    renderFlow->Prepare();
  }

  // //
  // // CREATE WORKFLOW RESOURCES
  // ----------------------------------------------- PC_WARN("Expensive
  // initialization operation: Creating workflow Vulkan "
  //         "resources! Should only be done once per workflow object init.")
};

// Sort materials, allocate descriptor sets, vk buffers, index buffers &
// create pipelines
void RendererVk::CreateRenderFlowResources() {
  //
  // Create VMA Allocator
  ContextVk::MemoryAllocator()->CreateVMAAllocator();

  // TODO:
  // 1. Group similar materials and save in a hashmap
  // 2. Create Vulkan resources:
  //    - Camera UBO memory (outside all loops)
  //    - Basic UBO memory (per each unique material combo)
  //    - Pbr UBO memory (per each unique material combo)
  //    - Model matrix UBO memory (per each game object)
  //    - (One big) Vertex buffer memory (submeshes, basic & pbr)
  //    - (One big) Index buffer memory
  //    - Descriptor sets (for every frame)

  // RenderFlowVk::AllocateMemory();

  for (auto &renderFlow : s_renderFlows) {
    renderFlow->AllocateVkVertexBuffers();  // VMA - Extract from meshes
    renderFlow->AllocateVkIndexBuffers();   // VMA - Extract from meshes
    renderFlow->AllocateVkUniformBuffers(); // VMA - Extract from -
                                            // 1. Camera
                                            // 2. Mesh
                                            // 3. Mesh material
    // -------------

    renderFlow->CreateDescriptorSetLayouts(); // Done
    renderFlow->CreateDescriptorPool();       // Move to factory
    renderFlow->CreateDescriptorSets();       // Move to factory
    renderFlow->CreatePipelines();            // ALMOST DONE
  }
};

void RendererVk::AssignSceneObjectsToRenderFlows() {
  for (auto &scene : m_sceneLibrary.GetScenes()) {
    for (auto &node : scene->GetGameObjects()) {
      ProcessGameObjectNode(node); // Recursive (FYI)
    };
  };
};

void RendererVk::ProcessGameObjectNode(GameObject *node) {
  switch (node->GetType()) {
  case Popcorn::Gfx::GameObjectTypes::Mesh: {
    // Add Mesh
    auto *mesh = static_cast<Mesh *>(node);
    for (auto &basicSubmesh : mesh->GetSubmeshes<MaterialTypes::BasicMat>()) {
      RenderFlowVk::AddSubmesh(&basicSubmesh);
    };
    for (auto &pbrSubmesh : mesh->GetSubmeshes<MaterialTypes::PbrMat>()) {
      RenderFlowVk::AddSubmesh(&pbrSubmesh);
    };
  } break;
  case Popcorn::Gfx::GameObjectTypes::Camera:
    // Add Camera (to extract projection & view matrices)
    break;
  case Popcorn::Gfx::GameObjectTypes::Empty:
    // Workflows have nothing to do with this type as of now
    break;
  case Popcorn::Gfx::GameObjectTypes::None:
    PC_ERROR("Wrong gameObject type", "RendererVk")
    break;
  }

  for (auto &child : node->GetChildren()) {
    ProcessGameObjectNode(child);
  }
};

RenderFlowVk *RendererVk::GetRenderFlow(const RenderFlows index) {
  return s_renderFlows[(int)index];
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
