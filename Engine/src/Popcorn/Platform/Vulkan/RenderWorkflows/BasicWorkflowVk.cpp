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
#include "VertexBuffer.h"
#include "VertexBufferVk.h"
#include <cstddef>
#include <cstdint>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

VertexBuffer::Layout BasicRenderWorkflowVk::s_vertexBufferLayout{};

#ifdef PC_DEBUG
constexpr bool showDrawCommandCount = false;
#endif

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
  VkVertexInputBindingDescription bindingDescription{};
  VertexBufferVk::GetDefaultVertexInputBindingDescription(bindingDescription,
                                                          s_vertexBufferLayout);
  bindingDescription.binding = 0;
  std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
  VertexBufferVk::GetDefaultVertexInputAttributeDescriptions(
      attributeDescriptions, s_vertexBufferLayout);

  pipelineState.vertexInputState.vertexBindingDescriptionCount = 1;
  pipelineState.vertexInputState.vertexAttributeDescriptionCount =
      static_cast<uint32_t>(attributeDescriptions.size());
  pipelineState.vertexInputState.pVertexBindingDescriptions =
      &bindingDescription;
  pipelineState.vertexInputState.pVertexAttributeDescriptions =
      attributeDescriptions.data();

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
  // SET VIEWPORT AND SCISSOR SIZE ---------------------------------------------
  VkViewport viewport{};
  VkRect2D scissor{};
  PipelineUtils::GetDefaultViewportAndScissorState(
      viewport, scissor, swapchainVkStn->GetSwapchainExtent());
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
#ifdef PC_DEBUG
  int drawCommandCount = 0;
#endif

  //
  // DRAW COMMAND --------------------------------------------------------------
  for (auto &material : m_materials) {
    // TODO: Choose pipeline according to material
    // BIND PIPELINE -----------------------------------------------------------
    m_colorPipelineVk.RecordBindCmdPipelineCommand(commandBuffer);

#ifdef PC_DEBUG
    if constexpr (showDrawCommandCount) {
      ++drawCommandCount;
    };
#endif

    VkBuffer vertexBuffers[] = {m_vkVertexBuffer};

    // TODO: Logic for multiple vertex buffers with VMA
    // BIND VERTEX BUFFERS ---------------------------------------------------
    VertexBufferVk::RecordBindVkBuffersCommand(commandBuffer, vertexBuffers,
                                               m_vkBufferOffsets.data(),
                                               m_meshes.size());

    for (auto &mesh : m_meshes) {
      vkCmdDraw(commandBuffer, mesh->GetVertexBuffer().GetCount(), 1, 0, 0);
    }
  }

  //
  // END RENDER PASS -----------------------------------------------------------
  m_basicRenderPassVk.RecordEndRenderPassCommand(commandBuffer);

#ifdef PC_DEBUG
  if constexpr (showDrawCommandCount) {
    PC_WARN("Draw command issued " << drawCommandCount << " times.")
  };
#endif
};

void BasicRenderWorkflowVk::AddMeshToWorkflow(Mesh *mesh) {
  auto ptr = std::find(m_meshes.begin(), m_meshes.end(), mesh);

  if (ptr != m_meshes.end()) {
    PC_WARN("Mesh already exists in m_meshes!")
    return;
  };

  if (mesh->GetVertexBuffer().GetLayout() != s_vertexBufferLayout) {
    PC_ERROR(
        "Mesh vertex layout doesn't match with BasicWorkflow vertex layout",
        "BasicRenderWorkflow")
    return;
  };

  auto &material = mesh->GetMaterial();

  if (material.GetMaterialType() != MaterialTypes::BasicMat) {
    PC_ERROR("Attempt to add a mesh to basicRenderWorkflow that is not "
             "MaterialTypes::BasicMat",
             "BasicRenderWorkflow");
    return;
  };

  m_meshes.push_back(mesh);

  // Each material can potentially have the same material type but different
  // descriptor sets (shaders, textures ..etc)
  RegisterMaterial(&material);
}

void BasicRenderWorkflowVk::AllocateVkVertexBuffers() {
  m_vkBufferOffsets.resize(m_meshes.size());

  VkDeviceSize currentOffset = 0;

  // Get the offsets of meshes & the total size of the buffers (a.k.a.
  // currentOffset)
  for (int i = 0; i < m_meshes.size(); ++i) {
    VertexBufferVk &vertexBuffer =
        reinterpret_cast<VertexBufferVk &>(m_meshes[i]->GetVertexBuffer());
    m_vkBufferOffsets[i] = currentOffset;
    currentOffset += vertexBuffer.GetSize();
  }

  VertexBufferVk::AllocateVkBuffer(m_vkVertexBuffer, m_vertexBufferMemory,
                                   currentOffset);

  void *data =
      VertexBufferVk::MapVkMemoryToCPU(m_vertexBufferMemory, 0, currentOffset);

  for (int i = 0; i < m_meshes.size(); ++i) {
    auto &vertexBuffer =
        static_cast<VertexBufferVk &>(m_meshes[i]->GetVertexBuffer());
    VertexBufferVk::CopyToVkMemory(
        m_vertexBufferMemory, (byte_t *)data + m_vkBufferOffsets[i],
        vertexBuffer.GetBufferData(), vertexBuffer.GetSize());
  }

  VertexBufferVk::UnmapVkMemoryFromCPU(m_vertexBufferMemory);
};

void BasicRenderWorkflowVk::CleanUp() {
  auto &device = DeviceVk::Get()->GetDevice();
  auto *framebuffersVkStn = FramebuffersVk::Get();

  // Cleanup vertex buffer memory
  VertexBufferVk::DestroyVkBuffer(m_vkVertexBuffer, m_vertexBufferMemory);
  m_vkBufferOffsets.clear();

  // Cleanup pipelines
  m_colorPipelineVk.CleanUp(device);

  // Cleanup render passes
  m_basicRenderPassVk.Destroy(device);
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
