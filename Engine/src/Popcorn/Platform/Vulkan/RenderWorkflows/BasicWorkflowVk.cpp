#include "BasicWorkflowVk.h"
#include "DeviceVk.h"
#include "FramebuffersVk.h"
#include "GfxPipelineVk.h"
#include "GlobalMacros.h"
#include "Material.h"
#include "PipelineVk.h"
#include "RenderPassVk.h"
#include "SwapchainVk.h"
#include "VertexBufferVk.h"
#include <cstdint>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

void BasicRenderWorkflowVk::CreatePipeline(Material &material) {
  //
  // --- MAIN PIPELINE ---------------------------------------------------
  auto *deviceVkStn = DeviceVk::Get();
  auto *swapchainVkStn = SwapchainVk::Get();
  auto &device = deviceVkStn->GetDevice();
  const auto &swapchainExtent = swapchainVkStn->GetSwapchainExtent();

  Buffer vertShaderBuffer = std::move(material.GetShaders()[0]);
  Buffer fragShaderBuffer = std::move(material.GetShaders()[1]);

  auto vertShaderModule = PC_CreateShaderModule(device, vertShaderBuffer);
  auto fragShaderModule = PC_CreateShaderModule(device, fragShaderBuffer);

  std::forward_list<VkShaderModule> shaderModules = {vertShaderModule,
                                                     fragShaderModule};

  // SET SHADER STAGES
  m_colorPipelineVk.SetShaderStagesMask(static_cast<ShaderStages>(
      ShaderStages::VertexBit | ShaderStages::FragmentBit));

  m_colorPipelineVk.CreateShaderStageCreateInfos(shaderModules);

  // SET FIXED FUNCTION PIPELINE STATE & SET LAYOUT
  GfxPipelineState pipelineState{};
  PipelineUtils::GetDefaultDynamicState(pipelineState.dynamicState);
  PipelineUtils::GetDefaultVertexInputState(pipelineState.vertexInputState);

  // Vertex input descriptions
  // VkVertexInputBindingDescription bindingDescription{};
  // VertexBufferVk::GetDefaultVertexInputBindingDescription(bindingDescription,
  //                                                         m_vertexLayout);
  // bindingDescription.binding = 0;
  // std::vector<VkVertexInputAttributeDescription> attrDescriptions;
  // VertexBufferVk::GetDefaultVertexInputAttributeDescriptions(attrDescriptions,
  //                                                            m_vertexLayout);

  // auto bindingDescription = Vertex::getBindingDescription();
  // auto attributeDescriptions = Vertex::getAttributeDescriptions();
  // pipelineState.vertexInputState.vertexBindingDescriptionCount = 1;

  PipelineUtils::GetDefaultInputAssemblyState(pipelineState.inputAssemblyState);
  PipelineUtils::GetDefaultViewportState(pipelineState.viewportState);
  PipelineUtils::GetDefaultRasterizationState(pipelineState.rasterizationState);
  PipelineUtils::GetDefaultMultisampleState(pipelineState.multisampleState);
  PipelineUtils::GetDefaultDepthStencilState(pipelineState.depthStencilState);
  PipelineUtils::GetDefaultColorBlendingState(pipelineState.colorBlendState);
  PipelineUtils::GetDefaultPipelineLayoutCreateInfo(
      pipelineState.pipelineLayout);

  // CREATE PIPELINE LAYOUT
  m_colorPipelineVk.CreatePipelineLayout(device, pipelineState.pipelineLayout);

  // CREATE VULKAN PIPELINE
  m_colorPipelineVk.CreateVkPipeline(device, pipelineState,
                                     m_basicRenderPassVk.GetVkRenderPass());

  // DESTROY SHADER MODULES
  PC_DestroyShaderModule(device, vertShaderModule);
  PC_DestroyShaderModule(device, fragShaderModule);

  //
  // --- OTHER PIPELINE --------------------------------------------------
};

void BasicRenderWorkflowVk::CreateRenderPass() {
  //
  // ----------------------------------------------------------------
  // --- Basic RenderPass -------------------------------------------
  //
  // ATTACHMENT DESCRIPTIONS ----------------------------------------
  VkAttachmentDescription colorAttachment{};
  RenderPassVk::GetDefaultAttachmentDescription(colorAttachment);

  auto *swapchainVk = SwapchainVk::Get();
  colorAttachment.format = swapchainVk->GetSwapchainImageFormat();

  std::vector<VkAttachmentDescription> attachments{}; // size 0
  attachments.emplace_back(colorAttachment);          // size 1

  //
  // ATTACHMENT REFERENCES ------------------------------------------
  VkAttachmentReference colorAttachmentRef{};
  RenderPassVk::GetAttachmentRef(colorAttachmentRef, 0); // index 0

  std::vector<VkAttachmentReference> attachmentRefs{}; // size 0
  attachmentRefs.emplace_back(colorAttachmentRef);     // size 1

  //
  // SUBPASSES ------------------------------------------------------
  VkSubpassDescription subpass1{};
  RenderPassVk::GetDefaultSubpassDescription(subpass1);
  subpass1.pColorAttachments = attachmentRefs.data();

  // INFO: The subpass attachment layouts are automatically set implicitly  with
  // the help of some internal dependencies from user-specified "in & out
  // attachment types" in the attachment data. But when you're writing to a
  // Swapchain image (in case of final render), you need to let it know to wait
  // until the image is actually available. So we create an external dependency
  // to ask the renderpass to wait at the required "stage" (in this case,
  // color-output stage).
  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  // VkSubpassDescription subpass2{};
  // RenderPassVk::GetDefaultSubpassDescription(subpass2);

  std::vector<VkSubpassDescription> subpasses{}; // size 0
  subpasses.emplace_back(subpass1);              // size 1

  //
  // CREATE RENDER PASS INFO ----------------------------------------
  VkRenderPassCreateInfo renderPass1CreateInfo{};
  RenderPassVk::SetDefaultRenderPassCreateInfo(renderPass1CreateInfo);

  // Dependencies -- For making the renderpass to wait on the color output
  // pipeline stage triggered by "swapchain image available" semaphore
  renderPass1CreateInfo.dependencyCount = 1;
  renderPass1CreateInfo.pDependencies = &dependency;

  renderPass1CreateInfo.pAttachments = attachments.data();
  renderPass1CreateInfo.pSubpasses = subpasses.data();

  //
  // CREATE VULKAN RENDER PASS --------------------------------------
  m_basicRenderPassVk.Create(renderPass1CreateInfo,
                             DeviceVk::Get()->GetDevice());
};

void BasicRenderWorkflowVk::CreateFramebuffers() {
  SwapchainVk::Get()->CreateSwapchainFramebuffers(
      DeviceVk::Get()->GetDevice(), m_basicRenderPassVk.GetVkRenderPass());
};

void BasicRenderWorkflowVk::CleanUp() {

  auto &device = DeviceVk::Get()->GetDevice();
  auto *framebuffersVkStn = FramebuffersVk::Get();

  // Cleanup pipelines
  m_colorPipelineVk.CleanUp(device);

  // Cleanup render passes
  m_basicRenderPassVk.Destroy(device);
};

void BasicRenderWorkflowVk::RecordRenderCommands(
    const Scene &scene, const VkCommandBuffer &commandBuffer,
    const uint32_t imageIndex) {
  // auto &renderPass = m_basicRenderPassVk.GetVkRenderPass();
  // PC_VK_NULL_CHECK(renderPass);
  auto *swapchainVkStn = SwapchainVk::Get();

  VkRenderPassBeginInfo renderPassCreateInfo{};

  //
  // BEGIN RENDER PASS ---------------------------------------------------------
  m_basicRenderPassVk.GetDefaultCmdBeginRenderPassInfo(
      swapchainVkStn->GetSwapchainFramebuffers()[imageIndex],
      swapchainVkStn->GetSwapchainExtent(), renderPassCreateInfo);
  m_basicRenderPassVk.RecordBeginRenderPassCommand(commandBuffer,
                                                   renderPassCreateInfo);

  //
  // BIND PIPELINE -------------------------------------------------------------
  m_colorPipelineVk.RecordBindCmdPipelineCommand(commandBuffer);

  //
  // SET VIEWPORT AND SCISSOR SIZE ---------------------------------------------
  VkViewport viewport{};
  VkRect2D scissor{};
  PipelineUtils::GetDefaultViewportAndScissorState(
      viewport, scissor, swapchainVkStn->GetSwapchainExtent());
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

  //
  // DRAW COMMAND --------------------------------------------------------------
  auto &sceneMaterials = scene.GetSceneMaterials();

  for (auto &material : sceneMaterials) {
    auto &linkedMeshes = material->GetLinkedMeshes();
    for (auto &linkedMesh : linkedMeshes) {
      // TODO: inject vertex data instead of hardcoding
      vkCmdDraw(commandBuffer, 3, 1, 0, 0);
    }
  }

  //
  // END RENDER PASS -----------------------------------------------------------
  m_basicRenderPassVk.RecordEndRenderPassCommand(commandBuffer);
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
