#include "RendererVk.h"
#include "PipelineVk.h"
#include "Pipelines/GfxPipelineVk.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Helpers.h"
#include "RenderPasses/RenderPassVk.h"
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
  const auto &swapchainSupportDetails =
      m_deviceVk.GetSwapchainSupportDetails(surface);
  const auto &queueFamilyIndices = m_deviceVk.GetQueueFamilyIndices(surface);

  // CREATE SWAPCHAIN --------------------------------------------------------
  m_swapchainVk.CreateSwapchain(device, swapchainSupportDetails, osWindow,
                                surface, queueFamilyIndices);
};

// TODO: Move this function to a separate class
void RendererVk::CreateTrianglePipeline() {
  const auto &device = m_deviceVk.GetDevice();
  const auto &swapchainExtent = m_swapchainVk.GetSwapchainExtent();

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
  trianglePipeline.SetShaderStagesMask(
      static_cast<ShaderStages>(ShaderStages::Vertex | ShaderStages::Fragment));
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

  // ATTACHMENT DESCRIPTIONS
  VkAttachmentDescription colorAttachment{};
  RenderPassVk::GetDefaultAttachmentDescription(colorAttachment);
  colorAttachment.format = m_swapchainVk.GetSwapchainImageFormat();

  std::vector<VkAttachmentDescription> attachments{}; // size 0
  attachments.emplace_back(colorAttachment);          // size 1

  // ATTACHMENT REFERENCES
  VkAttachmentReference colorAttachmentRef{};
  RenderPassVk::GetAttachmentRef(colorAttachmentRef, 0); // index 0

  std::vector<VkAttachmentReference> attachmentRefs{}; // size 0
  attachmentRefs.emplace_back(colorAttachmentRef);     // size 1

  // SUBPASSES
  VkSubpassDescription subpass1{};
  RenderPassVk::GetDefaultSubpassDescription(subpass1);
  subpass1.pColorAttachments = attachmentRefs.data();

  // VkSubpassDescription subpass2{};
  // RenderPassVk::GetDefaultSubpassDescription(subpass2);

  std::vector<VkSubpassDescription> subpasses{}; // size 0
  subpasses.emplace_back(subpass1);              // size 1

  // CREATE RENDER PASS
  VkRenderPassCreateInfo renderPass1CreateInfo{};
  RenderPassVk::SetDefaultRenderPassCreateInfo(renderPass1CreateInfo);
  renderPass1CreateInfo.pSubpasses = subpasses.data();

  RenderPassVk renderPass1;

  renderPass1.Create(renderPass1CreateInfo);
  renderPass1.Destroy();
};

void RendererVk::VulkanDestroy() {
  const auto &instance = m_deviceVk.GetInstance();
  const auto &device = m_deviceVk.GetDevice();

  m_swapchainVk.CleanUp(device);
  m_surfaceVk.CleanUp(instance);
  m_deviceVk.CleanUp();
};

void RendererVk::DrawFrame(const Scene &scene) const {};
bool RendererVk::OnFrameBfrResize(FrameBfrResizeEvent &) { return true; };

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
