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
  m_basicGfxPipelineVk.SetShaderStagesMask(static_cast<ShaderStages>(
      ShaderStages::VertexBit | ShaderStages::FragmentBit));
  std::vector<VkPipelineShaderStageCreateInfo> shaderStages =
      m_basicGfxPipelineVk.CreateShaderStages(shaderModules);

  // SET FIXED FUNCTION PIPELINE STATE & SET LAYOUT
  GfxPipelineState pipelineState{};
  m_basicGfxPipelineVk.GetDefaultPipelineState(pipelineState);
  // m_basicGfxPipeline.GetDefaultPipelineLayoutCreateInfo(
  //     createInfo.pipelineLayout); // Already in
  //     GetDefaultPipelineCreateInfo()
  m_basicGfxPipelineVk.SetShaderStageCreateInfos(shaderStages);
  m_basicGfxPipelineVk.SetPipelineLayout(device, pipelineState.pipelineLayout);

  // CREATE PIPELINE
  m_basicGfxPipelineVk.CreateVkPipeline(device, pipelineState,
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

  // VkSubpassDescription subpass2{};
  // RenderPassVk::GetDefaultSubpassDescription(subpass2);

  std::vector<VkSubpassDescription> subpasses{}; // size 0
  subpasses.emplace_back(subpass1);              // size 1

  //
  // CREATE RENDER PASS INFO ----------------------------------------
  VkRenderPassCreateInfo renderPass1CreateInfo{};
  RenderPassVk::SetDefaultRenderPassCreateInfo(renderPass1CreateInfo);
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
  m_basicGfxPipelineVk.Destroy(device);

  // Cleanup render passes
  m_basicRenderPassVk.Destroy(device);
};

void BasicRenderWorkflowVk::RecordRenderCommands(
    const VkCommandBuffer commandBuffer, uint32_t imageIndex) {
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
  m_basicGfxPipelineVk.RecordBindCmdPipelineCommand(commandBuffer);

  //
  // SET VIEWPORT AND SCISSOR SIZE ---------------------------------------------
  VkViewport viewport{};
  VkRect2D scissor{};
  m_basicGfxPipelineVk.GetDefaultViewportAndScissorState(
      viewport, scissor, swapchainVkStn->GetSwapchainExtent());
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

  //
  // TODO: Abstract this accordingly
  // DRAW COMMAND --------------------------------------------------------------
  vkCmdDraw(commandBuffer, 3, 1, 0, 0);

  //
  // END RENDER PASS -----------------------------------------------------------
  m_basicRenderPassVk.RecordEndRenderPassCommand(commandBuffer);
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
