#include "RendererVk.h"
#include "Pipelines/GfxPipelineVk.h"
#include "Popcorn/Core/Base.h"
#include "Shader.h"
#include "Sources.h"
#include <cstring>
#include <forward_list>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

RendererVk::RendererVk(const Window &appWin) : Renderer(appWin) {
  PC_PRINT("CREATED", TagType::Constr, "RENDERER-VK");
};

RendererVk::~RendererVk() {
  PC_PRINT("DESTROYED", TagType::Destr, "RENDERER-VULKAN");
};

void RendererVk::VulkanInit() {
  GLFWwindow *osWindow = static_cast<GLFWwindow *>(m_AppWin.GetOSWindow());

  // CREATE INSTANCE, SET UP DEBUGGING LAYERS --------------------------------
  m_deviceVk.CreateInstance({"Vulkan App", 1, 0, 0});
  m_deviceVk.SetupDebugMessenger();

  const auto &instance = m_deviceVk.GetInstance();

  // CREATE WINDOW SURFACE ---------------------------------------------------
  m_surfaceVk.CreateWindowSurface(instance, osWindow);

  const auto &surface = m_surfaceVk.GetSurface();

  // CREATE PHYSICAL & LOGICAL DEVICE ----------------------------------------
  m_deviceVk.PickPhysicalDevice(surface);
  m_deviceVk.CreateLogicalDevice(surface);

  const auto &device = m_deviceVk.GetDevice();
  const auto &swapChainSupportDetails =
      m_deviceVk.GetSwapChainSupportDetails(surface);
  const auto &queueFamilyIndices = m_deviceVk.GetQueueFamilyIndices(surface);

  // CREATE SWAPCHAIN --------------------------------------------------------
  m_swapChainVk.CreateSwapChain(device, swapChainSupportDetails, osWindow,
                                surface, queueFamilyIndices);
};

void RendererVk::CreateVulkanPipeline() {
  const auto &device = m_deviceVk.GetDevice();
  const auto &swapchainExtent = m_swapChainVk.GetSwapchainExtent();

  // PIPELINE
  GfxPipelineVk trianglePipeline{};

  VkPipelineDynamicStateCreateInfo dynamicState{};
  VkPipelineVertexInputStateCreateInfo vertexInputState{};
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
  VkPipelineViewportStateCreateInfo viewportState{};
  VkPipelineRasterizationStateCreateInfo rasterizationState{};
  VkPipelineMultisampleStateCreateInfo multisampleState{};
  VkPipelineDepthStencilStateCreateInfo depthStencilState{};
  VkPipelineColorBlendStateCreateInfo colorBlendState{};

  Shader s;
  auto vertShaderBuffer =
      s.ReadSpvFile(PC_SHADER_SOURCE_MAP[VertShaderTriangle]);
  auto fragShaderBuffer =
      s.ReadSpvFile(PC_SHADER_SOURCE_MAP[FragShaderTriangle]);

  auto vertShaderModule =
      trianglePipeline.CreateShaderModule(device, vertShaderBuffer);
  auto fragShaderModule =
      trianglePipeline.CreateShaderModule(device, fragShaderBuffer);

  std::forward_list<VkShaderModule> shaderModules = {vertShaderModule,
                                                     fragShaderModule};
  trianglePipeline.SetShaderStages(
      static_cast<ShaderStages>(ShaderStages::Vertex | ShaderStages::Fragment));
  auto shaderStages = trianglePipeline.CreateShaderStages(shaderModules);

  trianglePipeline.SetDefaultDynamicState(dynamicState);
  trianglePipeline.SetDefaultVertexInputState(vertexInputState);
  trianglePipeline.SetDefaultInputAssemblyState(inputAssemblyState);
  trianglePipeline.SetDefaultViewportState(viewportState, swapchainExtent);
  trianglePipeline.SetDefaultRasterizationState(rasterizationState);
  trianglePipeline.SetDefaultMultisampleState(multisampleState);
  trianglePipeline.SetDefaultDepthStencilState(depthStencilState);
  trianglePipeline.SetDefaultColorBlendingState(colorBlendState);
  trianglePipeline.SetDefaultPipelineLayout(device);

  trianglePipeline.DestroyShaderModule(device, vertShaderModule);
  trianglePipeline.DestroyShaderModule(device, fragShaderModule);
};

void RendererVk::VulkanDestroy() {
  const auto &instance = m_deviceVk.GetInstance();
  const auto &device = m_deviceVk.GetDevice();

  // trianglePipeline.DestroyPipelineLayout(device);

  m_swapChainVk.CleanUp(device);
  m_surfaceVk.CleanUp(instance);
  m_deviceVk.CleanUp();
};

void RendererVk::DrawFrame() {};
void RendererVk::PresentFrame() {};
bool RendererVk::OnFrameBfrResize(FrameBfrResizeEvent &) { return true; };

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
