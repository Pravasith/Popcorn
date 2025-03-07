#include "BasicWorkflowVk.h"
#include "BufferObjects.h"
#include "DeviceVk.h"
#include "FramebuffersVk.h"
#include "GfxPipelineVk.h"
#include "GlobalMacros.h"
#include "Material.h"
#include "PipelineVk.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Buffer.h"
#include "Popcorn/Core/Helpers.h"
#include "RenderPassVk.h"
#include "SwapchainVk.h"
#include "VertexBufferVk.h"
#include <cstdint>
#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

BufferDefs::Layout BasicRenderWorkflowVk::s_vertexBufferLayout{};

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

    // TODO: Logic for multiple vertex buffers with VMA
    // BIND VERTEX BUFFERS ---------------------------------------------------

    for (size_t i = 0; i < m_meshes.size(); ++i) {
      // std::cout << "Drawing mesh " << i << " with offset "
      //           << m_vkBufferOffsets[i] << "\n";

      VkBuffer vertexBuffers[] = {m_vkVertexBuffer};
      VkDeviceSize offsets[] = {m_vertexBufferOffsets[i]};

      BufferVkUtils::RecordBindVkVertexBuffersCommand(
          commandBuffer, vertexBuffers, offsets, 1);

      // TODO: Change it to variant logic
      BufferVkUtils::RecordBindVkIndexBufferCommand<uint16_t>(
          commandBuffer, &m_vkIndexBuffer, m_indexBufferOffsets[i]);

      // vkCmdDraw(commandBuffer, m_meshes[i]->GetVertexBuffer().GetCount(), 1,
      // 0, 0);

      vkCmdDrawIndexed(commandBuffer, m_meshes[i]->GetIndexBuffer().GetCount(),
                       1, 0,
                       // m_vertexBufferOffsets[i],
                       0,
                       //
                       0);
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

//
// --------------------------------------------------------------------------
// --- VERTEX BUFFER ALLOCATION ---------------------------------------------
//
void BasicRenderWorkflowVk::AllocateVkVertexBuffers() {
  m_vertexBufferOffsets.resize(m_meshes.size());

  VkDeviceSize currentOffset = 0;

  // Get the offsets of meshes & the total size of the buffers (a.k.a.
  // currentOffset)
  for (int i = 0; i < m_meshes.size(); ++i) {
    VertexBufferVk &vertexBuffer =
        reinterpret_cast<VertexBufferVk &>(m_meshes[i]->GetVertexBuffer());
    m_vertexBufferOffsets[i] = currentOffset;
    currentOffset += vertexBuffer.GetSize();
  }

  VkBuffer stagingVertexBuffer;
  VkDeviceMemory stagingVertexBufferMemory;

  // Allocate staging buffer
  VkBufferCreateInfo bufferInfo{};
  BufferVkUtils::GetDefaultVkBufferState(bufferInfo, currentOffset);
  bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  BufferVkUtils::AllocateVkBuffer(stagingVertexBuffer,
                                  stagingVertexBufferMemory, bufferInfo,
                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  // struct Vertex {
  //   glm::vec2 pos;
  //   glm::vec3 color;
  //   std::string Print() {
  //     std::stringstream ss;
  //     ss << pos.x << ", " << pos.y << "; " << color.r << ", " << color.g <<
  //     ", "
  //        << color.b;
  //
  //     return ss.str();
  //   };
  // };

  void *data = BufferVkUtils::MapVkMemoryToCPU(stagingVertexBufferMemory, 0,
                                               currentOffset);

  for (int i = 0; i < m_meshes.size(); ++i) {
    VertexBufferVk &vertexBuffer =
        static_cast<VertexBufferVk &>(m_meshes[i]->GetVertexBuffer());

    // vertexBuffer.PrintBuffer<Vertex>();
    // PC_WARN(vertexBuffer.GetSize())

    BufferVkUtils::CopyBufferCPUToGPU(stagingVertexBufferMemory,
                                      // Dest ptr
                                      (byte_t *)data + m_vertexBufferOffsets[i],
                                      // Src ptr
                                      vertexBuffer.GetBufferData(),
                                      // Size
                                      vertexBuffer.GetSize());
  }

  // Vertex *vertices = static_cast<Vertex *>(data);
  // for (size_t i = 0; i < 6; ++i) { // 6 vertices (2 meshes × 3 vertices)
  //   std::cout << "Vertex " << i << ": " << vertices[i].Print() << "\n";
  // }

  BufferVkUtils::UnmapVkMemoryFromCPU(stagingVertexBufferMemory);

  //
  // Allocate actual vertex buffer
  bufferInfo.usage =
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  BufferVkUtils::AllocateVkBuffer(m_vkVertexBuffer, m_vkVertexBufferMemory,
                                  bufferInfo,
                                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  //
  // Copy staging buffer data to local buffers
  BufferVkUtils::CopyBufferGPUToGPU(stagingVertexBuffer, m_vkVertexBuffer,
                                    currentOffset);

  // Cleanup staging buffers
  auto &device = DeviceVk::Get()->GetDevice();
  vkDestroyBuffer(device, stagingVertexBuffer, nullptr);
  vkFreeMemory(device, stagingVertexBufferMemory, nullptr);
};

//
// --------------------------------------------------------------------------
// --- INDEX BUFFER ALLOCATION ----------------------------------------------
//
void BasicRenderWorkflowVk::AllocateVkIndexBuffers() {
  m_indexBufferOffsets.resize(m_meshes.size());

  VkDeviceSize currentOffset = 0;

  // Get the offsets of meshes & the total size of the buffers (a.k.a.
  // currentOffset)
  for (int i = 0; i < m_meshes.size(); ++i) {
    IndexBuffer<uint16_t> &indexBuffer = m_meshes[i]->GetIndexBuffer();
    m_indexBufferOffsets[i] = currentOffset;
    currentOffset += indexBuffer.GetSize();
  }

  VkBuffer stagingIndexBuffer;
  VkDeviceMemory stagingIndexBufferMemory;

  // Allocate staging buffer
  VkBufferCreateInfo bufferInfo{};
  BufferVkUtils::GetDefaultVkBufferState(bufferInfo, currentOffset);
  bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  BufferVkUtils::AllocateVkBuffer(stagingIndexBuffer, stagingIndexBufferMemory,
                                  bufferInfo,
                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  void *data = BufferVkUtils::MapVkMemoryToCPU(stagingIndexBufferMemory, 0,
                                               currentOffset);

  for (int i = 0; i < m_meshes.size(); ++i) {
    IndexBuffer<uint16_t> &indexBuffer = m_meshes[i]->GetIndexBuffer();

    // vertexBuffer.PrintBuffer<Index>();
    // PC_WARN(vertexBuffer.GetSize())

    BufferVkUtils::CopyBufferCPUToGPU(stagingIndexBufferMemory,
                                      // Dest ptr
                                      (byte_t *)data + m_indexBufferOffsets[i],
                                      // Src ptr
                                      indexBuffer.GetBufferData(),
                                      // Size
                                      indexBuffer.GetSize());
  }

  // Index *vertices = static_cast<Index *>(data);
  // for (size_t i = 0; i < 6; ++i) { // 6 vertices (2 meshes × 3 vertices)
  //   std::cout << "Index " << i << ": " << vertices[i].Print() << "\n";
  // }

  BufferVkUtils::UnmapVkMemoryFromCPU(stagingIndexBufferMemory);

  //
  // Allocate actual index buffer
  bufferInfo.usage =
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
  BufferVkUtils::AllocateVkBuffer(m_vkIndexBuffer, m_vkIndexBufferMemory,
                                  bufferInfo,
                                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  //
  // Copy staging buffer data to local buffers
  BufferVkUtils::CopyBufferGPUToGPU(stagingIndexBuffer, m_vkIndexBuffer,
                                    currentOffset);

  // Cleanup staging buffers
  auto &device = DeviceVk::Get()->GetDevice();
  vkDestroyBuffer(device, stagingIndexBuffer, nullptr);
  vkFreeMemory(device, stagingIndexBufferMemory, nullptr);
};

void BasicRenderWorkflowVk::CleanUp() {
  auto &device = DeviceVk::Get()->GetDevice();
  auto *framebuffersVkStn = FramebuffersVk::Get();

  // Cleanup index buffer memory
  BufferVkUtils::DestroyVkBuffer(m_vkIndexBuffer, m_vkIndexBufferMemory);
  m_indexBufferOffsets.clear();

  // Cleanup vertex buffer memory
  BufferVkUtils::DestroyVkBuffer(m_vkVertexBuffer, m_vkVertexBufferMemory);
  m_vertexBufferOffsets.clear();

  // Cleanup pipelines
  m_colorPipelineVk.CleanUp(device);

  // Cleanup render passes
  m_basicRenderPassVk.Destroy(device);
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
