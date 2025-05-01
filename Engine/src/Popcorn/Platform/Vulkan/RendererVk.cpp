#include "RendererVk.h"
#include "CommandPoolVk.h"
#include "ContextVk.h"
#include "GameObject.h"
#include "Material.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Helpers.h"
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
  RenderFlowVk::DestroyVBOsAndIBOs();

  for (auto &renderFlow : s_renderFlows) {
    renderFlow->CleanUp();
    delete renderFlow;
    renderFlow = nullptr;
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
};

void RendererVk::PrepareRenderFlows() {
  for (auto &renderFlow : s_renderFlows) {
    PC_WARN("Preparing renderflow...")
    renderFlow->Prepare(); // Creates Vulkan:
                           //   - Attachments
                           //   - RenderPass
                           //   - Framebuffer
  }
};

// Sort materials, allocate descriptor sets, vk buffers, index buffers &
// create pipelines
void RendererVk::CreateRenderFlowResources() {
  //
  // CREATE WORKFLOW RESOURCES -----------------------------------------------
  PC_WARN("Expensive initialization operation: Creating workflow Vulkan "
          "resources! Should only be done once")

  //
  // Create VMA Allocator
  ContextVk::MemoryAllocator()->CreateVMAAllocator();

  // Allocates vulkan buffers
  RenderFlowVk::AllocVBOsAndIBOsMemory();

  for (auto &renderFlow : s_renderFlows) {
    renderFlow->AllocUBOsMemory();
    //
    // -------
    // - Camera UBO - viewProj -- per frame (lvl 0)
    // -------
    // - GameObj UBOs - worldMatrices (dynamic offsets) -- per submesh (lvl 2)
    // -------
    // - BasicMat UBOs - BaseColor vec3s (dynamic offsets) -- per mat (lvl 1)
    // -------
    // - PbrMat UBOs - A bunch of shit (dynamic offsets) -- per mat (lvl 1)
    // -------
    // - Lights UBOs - lightpos vec3s (dynamic offsets) -- per light
    // - Albedo Image+Sampler - just one
    // - Depth Image+Sampler - just one
    // - Normals Image+Sampler - just one
    // -------
    // - LitScene Image+Sampler - just one
    //
    // for (auto &[matId, submeshes] : submeshGroups) {
    //   for (Submesh<T> *submesh : submeshes) {
    //   }
    // };

    renderFlow->CreateAndAllocDescriptors(); // Static for now
    // renderFlow->CreatePipelines();
  }

  // TODOs:
  // - Create VMA buffers for UBOs
  // - Create Samplers for G-Buffer & LitScene-Buffer
  // - Bind(write/update) buffers to descriptor sets(or descriptors?)
  // - Copy scene updates(UBOs) via OnUpdate() to mapped bits

  // - Repeat the process for other renderflows
  // - Write lighting shaders
  // - Finish create pipelines
  // - Write render callback for renderflows
  // - Blender scene yaayyy!
};

void RendererVk::AssignSceneObjectsToRenderFlows() {
  for (auto &scene : m_sceneLibrary.GetScenes()) {
    for (auto &node : scene->GetGameObjects()) {
      ProcessGameObjectNode(node); // Recursive
    };
  };
};

void RendererVk::ProcessGameObjectNode(GameObject *node) { // Recursive
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
    ProcessGameObjectNode(child); // Recursive
  }
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
