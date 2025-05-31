#include "RendererVk.h"
#include "ContextVk.h"
#include "GameObject.h"
#include "Material.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Helpers.h"
#include "RenderFlows/CompositeRenderFlowVk.h"
#include "RenderFlows/GBufferRenderFlowVk.h"
#include "RenderFlows/LightingRenderFlowVk.h"
#include "RenderFlows/RenderFlowVk.h"
#include "Shader.h"
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
  ContextVk::Frame()->Draw(
      [&]() {
        for (auto &renderFlow : s_renderFlows) {
          renderFlow->OnSwapchainInvalidCb();
        }
      },
      [&](const uint32_t currentFrame) {
        RenderFlowVk::CopyDynamicUniformsToMemory(currentFrame);
      },
      [&](const uint32_t frameIndex, const uint32_t currentFrame) {
        for (auto &renderFlow : s_renderFlows) {
          renderFlow->RecordCommandBuffer(frameIndex, currentFrame);
        }
      });
};

bool RendererVk::OnFrameBufferResize(FrameBfrResizeEvent &) {
  ContextVk::Frame()->SetFrameBufferResized(true);
  return true;
};

//
// -------------------------------------------------------------------------
// --- PRIVATE METHODS -----------------------------------------------------

RendererVk::RendererVk(const Window &appWin) : Renderer(appWin) {
  PC_PRINT("CREATED", TagType::Constr, "RendererVk");
  CreateVulkanContext();
};

RendererVk::~RendererVk() {
  DestroyVulkanContext();
  PC_PRINT("DESTROYED", TagType::Destr, "RendererVk");
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

void RendererVk::DestroyRenderFlows() {
  for (auto &renderFlow : s_renderFlows) {
    renderFlow->DestroyPipelines();
  }

  ShaderLibrary::Destroy();

  RenderFlowVk::DestroySamplers();
  RenderFlowVk::FreeMemory();

  for (auto &renderFlow : s_renderFlows) {
    renderFlow->CleanUp();
    delete renderFlow;
    renderFlow = nullptr;
  }
  s_renderFlows.clear();
};

void RendererVk::PrepareRenderFlows() {
  for (auto &renderFlow : s_renderFlows) {
    PC_WARN("Preparing renderflow...")
    renderFlow->Prepare(); // Creates Vulkan:
                           //   - Attachments
                           //   - RenderPass
                           //   - Framebuffer
                           //   - Commandbuffers
  }
};

void RendererVk::CreateRenderFlowResources() {
  //
  // CREATE WORKFLOW RESOURCES -----------------------------------------------
  PC_WARN("Expensive initialization operation: Creating workflow Vulkan "
          "resources! Should only be done once")

  //
  // Create VMA Allocator
  ContextVk::MemoryAllocator()->CreateVMAAllocator(); // Automatically destroyed

  //
  // Allocate memory, samplers & load shaders in the shader library
  RenderFlowVk::AllocMemory();
  RenderFlowVk::CreateSamplers();
  RenderFlowVk::AllocShaders();
  RenderFlowVk::AllocGlobalDescriptors();

  //
  // Renderflow specific descriptors & pipelines
  for (auto &renderFlow : s_renderFlows) {
    renderFlow->AllocLocalDescriptors(); // Static for now
    renderFlow->CreatePipelines();
  }

  //
  // Unload shaders in the shader library
  RenderFlowVk::FreeShaders();
};

void RendererVk::AssignSceneObjectsToRenderFlows() {
  for (auto &scene : m_sceneLibrary.GetScenes()) {
    for (auto &node : scene->GetGameObjects()) {
      ProcessGameObjectNode(node); // Recursive
    };
  };
};

void RendererVk::ProcessGameObjectNode(GameObject *node) { // Recursive
  switch (node->GetGameObjectType()) {
  case Popcorn::Gfx::GameObjectTypes::Mesh: //
  {
    auto *mesh = static_cast<Mesh *>(node);
    for (auto &basicSubmesh : mesh->GetSubmeshes<MaterialTypes::BasicMat>()) {
      RenderFlowVk::RegisterMaterialAndSubmesh(&basicSubmesh);
    };
    for (auto &pbrSubmesh : mesh->GetSubmeshes<MaterialTypes::PbrMat>()) {
      RenderFlowVk::RegisterMaterialAndSubmesh(&pbrSubmesh);
    };
  } break;

  case Popcorn::Gfx::GameObjectTypes::Camera: //
  {
    auto *camera = static_cast<Camera *>(node);
    RenderFlowVk::AddCamera(camera);
  }
  // Add Camera (to extract projection & view matrices)
  break;

  case Popcorn::Gfx::GameObjectTypes::Empty: // Workflows have nothing to do
                                             // with this type as of now
  {
    auto *empty = static_cast<Empty *>(node);
    RenderFlowVk::AddEmpty(empty);
  } break;

  case Popcorn::Gfx::GameObjectTypes::Light: //
  {
    auto *light = static_cast<Light *>(node);
    RenderFlowVk::AddLight(light);
  } break;

  case Popcorn::Gfx::GameObjectTypes::None: //
  {
    PC_ERROR("Wrong gameObject type", "RendererVk")
  } break;
  }

  for (auto &child : node->GetChildren()) {
    ProcessGameObjectNode(child); // Recursive
  }
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
