#include "RendererVk.h"
#include "DeviceVk.h"
#include "PipelineVk.h"
#include "Pipelines/GfxPipelineVk.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Helpers.h"
#include "Shader.h"
#include "Sources.h"
#include "SurfaceVk.h"
#include "SwapchainVk.h"
#include <cstring>
#include <forward_list>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

DeviceVk *RendererVk::s_deviceVk = DeviceVk::Get();
SurfaceVk *RendererVk::s_surfaceVk = SurfaceVk::Get();
SwapchainVk *RendererVk::s_swapchainVk = SwapchainVk::Get();

std::vector<RenderWorkflowVk> RendererVk::s_renderWorkflows = {};

//
// -------------------------------------------------------------------------
// --- PUBLIC METHODS ------------------------------------------------------

void RendererVk::DrawFrame(const Scene &scene) const {};
bool RendererVk::OnFrameBfrResize(FrameBfrResizeEvent &) { return true; };
void RendererVk::CreateRenderWorkflows() {
  //
  // --- BASIC MATERIAL WORK FLOW ------------------------------------------
  // -----------------------------------------------------------------------
};

//
// -------------------------------------------------------------------------
// --- PRIVATE METHODS -----------------------------------------------------

RendererVk::RendererVk(const Window &appWin) : Renderer(appWin) {
  PC_PRINT("CREATED", TagType::Constr, "RENDERER-VK");
};

RendererVk::~RendererVk() {
  SwapchainVk::Destroy();
  SurfaceVk::Destroy();
  DeviceVk::Destroy();

  PC_PRINT("DESTROYED", TagType::Destr, "RENDERER-VULKAN");
};

void RendererVk::VulkanInit() {
  GLFWwindow *osWindow = static_cast<GLFWwindow *>(m_AppWin.GetOSWindow());

  // CREATE INSTANCE, SET UP DEBUGGING LAYERS --------------------------------
  s_deviceVk->CreateInstance({"Vulkan App", 1, 0, 0});
  s_deviceVk->SetupDebugMessenger();

  const auto &instance = s_deviceVk->GetVkInstance();

  // CREATE WINDOW SURFACE ---------------------------------------------------
  s_surfaceVk->CreateWindowSurface(instance, osWindow);

  const auto &surface = s_surfaceVk->GetSurface();

  // CREATE PHYSICAL & LOGICAL DEVICE ----------------------------------------
  s_deviceVk->PickPhysicalDevice(surface);
  s_deviceVk->CreateLogicalDevice(surface);

  const auto &device = s_deviceVk->GetDevice();
  const auto &swapchainSupportDetails =
      s_deviceVk->GetSwapchainSupportDetails(surface);
  const auto &queueFamilyIndices = s_deviceVk->GetQueueFamilyIndices(surface);

  // CREATE SWAPCHAIN --------------------------------------------------------
  s_swapchainVk->CreateSwapchain(device, swapchainSupportDetails, osWindow,
                                 surface, queueFamilyIndices);
};

// TODO: Move this function to a separate class
void RendererVk::CreateTrianglePipeline() {
  const auto &device = s_deviceVk->GetDevice();
  const auto &swapchainExtent = s_swapchainVk->GetSwapchainExtent();

  // CREATE SHADER MODULES
  auto vertShaderBuffer = Shader::ReadSpvFile(
      PC_SHADER_SOURCE_MAP[ShaderFiles::VertShaderTriangle]);
  auto fragShaderBuffer = Shader::ReadSpvFile(
      PC_SHADER_SOURCE_MAP[ShaderFiles::FragShaderTriangle]);

  auto vertShaderModule = PC_CreateShaderModule(device, vertShaderBuffer);
  auto fragShaderModule = PC_CreateShaderModule(device, fragShaderBuffer);

  std::forward_list<VkShaderModule> shaderModules = {vertShaderModule,
                                                     fragShaderModule};

  // CREATE BASIC PIPELINE
  GfxPipelineVk trianglePipeline{};

  // SET PIPELINE DEVICE
  trianglePipeline.SetDevice(device);

  // SET SHADER STAGES
  trianglePipeline.SetShaderStagesMask(static_cast<ShaderStages>(
      ShaderStages::VertexBit | ShaderStages::FragmentBit));
  std::vector<VkPipelineShaderStageCreateInfo> shaderStages =
      trianglePipeline.CreateShaderStages(shaderModules);

  // SET FIXED FUNCTION PIPELINE STATE & SET LAYOUT
  GfxPipelineCreateInfo createInfo{};
  trianglePipeline.GetDefaultPipelineCreateInfo(createInfo);
  trianglePipeline.SetShaderStageCreateInfos(shaderStages);
  trianglePipeline.SetPipelineLayout(createInfo.pipelineLayout);

  //

  // CREATE PIPELINE
  trianglePipeline.Create(createInfo);

  // DESTROY SHADER MODULES
  PC_DestroyShaderModule(device, vertShaderModule);
  PC_DestroyShaderModule(device, fragShaderModule);
};

// TODO: Move this function to a separate class
void RendererVk::CreateTriangleRenderPass() {

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
