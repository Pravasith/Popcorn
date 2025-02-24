#include "BasicWorkflowVk.h"
#include "DeviceVk.h"
#include "FramebuffersVk.h"
#include "GfxPipelineVk.h"
#include "GlobalMacros.h"
#include "Material.h"
#include "PipelineVk.h"
#include "Popcorn/Core/Base.h"
#include "RenderPassVk.h"
#include "SwapchainVk.h"
#include <cstdint>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

void BasicRenderWorkflowVk::CreateVkPipeline(Material &material) {
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
  std::vector<VkPipelineShaderStageCreateInfo> shaderStages =
      m_colorPipelineVk.CreateShaderStages(shaderModules);

  // SET FIXED FUNCTION PIPELINE STATE & SET LAYOUT
  GfxPipelineState pipelineState{};
  m_colorPipelineVk.GetDefaultPipelineState(pipelineState);
  // m_basicGfxPipeline.GetDefaultPipelineLayoutCreateInfo(
  //     createInfo.pipelineLayout); // Already in
  //     GetDefaultPipelineCreateInfo()
  m_colorPipelineVk.SetShaderStageCreateInfos(shaderStages);
  m_colorPipelineVk.SetPipelineLayout(device, pipelineState.pipelineLayout);

  // CREATE PIPELINE
  m_colorPipelineVk.CreateVkPipeline(device, pipelineState,
                                     m_basicRenderPassVk.GetVkRenderPass());

  // DESTROY SHADER MODULES
  PC_DestroyShaderModule(device, vertShaderModule);
  PC_DestroyShaderModule(device, fragShaderModule);
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
  auto *deviceSingleton = DeviceVk::Get();
  m_basicRenderPassVk.Create(renderPass1CreateInfo,
                             deviceSingleton->GetDevice());
};

void BasicRenderWorkflowVk::CreateFramebuffers() {
  auto *deviceVk = DeviceVk::Get();
  auto *swapchainVkStn = SwapchainVk::Get();
  auto *framebuffersVkStn = FramebuffersVk::Get();

  auto &swapchainImgViews = swapchainVkStn->GetSwapchainImageViews();
  auto &swapchainExtent = swapchainVkStn->GetSwapchainExtent();

  VkFramebufferCreateInfo createInfo;

  m_swapchainFramebuffers.resize(swapchainImgViews.size());

  for (size_t i = 0; i < swapchainImgViews.size(); ++i) {
    VkImageView attachments[] = {swapchainImgViews[i]};

    framebuffersVkStn->GetDefaultFramebufferState(createInfo);
    createInfo.renderPass = m_basicRenderPassVk.GetVkRenderPass();
    createInfo.pAttachments = attachments;
    createInfo.width = swapchainExtent.width;
    createInfo.height = swapchainExtent.height;

    // CREATE VK FRAMEBUFFER
    framebuffersVkStn->CreateVkFramebuffer(deviceVk->GetDevice(), createInfo,
                                           m_swapchainFramebuffers[i]);
  };
};

void BasicRenderWorkflowVk::CleanUp() {
  if (m_swapchainFramebuffers.size() == 0) {
    PC_ERROR("Tried to clear m_swapchainFramebuffers but size is 0!",
             "BasicWorkFlow")
  };

  auto *deviceSingleton = DeviceVk::Get();
  auto &device = deviceSingleton->GetDevice();
  auto *framebuffersVkStn = FramebuffersVk::Get();
  // auto *commandPoolVkStn = CommandPoolVk::Get();

  // Cleanup framebuffers
  for (auto &framebuffer : m_swapchainFramebuffers) {
    framebuffersVkStn->DestroyVkFramebuffer(device, framebuffer);
  };

  // Cleanup pipelines
  m_colorPipelineVk.Destroy(device);

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
      m_swapchainFramebuffers[imageIndex], swapchainVkStn->GetSwapchainExtent(),
      renderPassCreateInfo);
  m_basicRenderPassVk.RecordBeginRenderPassCommand(commandBuffer,
                                                   renderPassCreateInfo);

  //
  // BIND PIPELINE -------------------------------------------------------------
  m_colorPipelineVk.RecordBindCmdPipelineCommand(commandBuffer);

  //
  // SET VIEWPORT AND SCISSOR SIZE ---------------------------------------------
  VkViewport viewport{};
  VkRect2D scissor{};
  m_colorPipelineVk.GetDefaultViewportAndScissorState(
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
