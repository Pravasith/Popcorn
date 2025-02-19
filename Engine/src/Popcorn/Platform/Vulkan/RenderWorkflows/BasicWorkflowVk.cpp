#include "BasicWorkflowVk.h"
#include "GlobalMacros.h"
#include "RenderPassVk.h"
#include "SwapchainVk.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

void BasicRenderWorkflowVk::CreateRenderPass() {
  // ATTACHMENT DESCRIPTIONS
  VkAttachmentDescription colorAttachment{};
  RenderPassVk::GetDefaultAttachmentDescription(colorAttachment);

  auto swapchainVk = SwapchainVk::Get();
  colorAttachment.format = swapchainVk->GetSwapchainImageFormat();

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

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
