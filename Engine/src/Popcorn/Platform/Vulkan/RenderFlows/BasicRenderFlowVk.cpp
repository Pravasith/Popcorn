#include "RenderFlows/BasicRenderFlowVk.h"
#include "BufferObjects.h"
#include "BufferObjectsVk.h"
#include "ContextVk.h"
#include "DescriptorsVk.h"
#include "GfxPipelineVk.h"
#include "GlobalMacros.h"
#include "Material.h"
#include "PipelineVk.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Helpers.h"
#include "RenderPassVk.h"
#include "RendererVk.h"
#include <cstddef>
#include <cstdint>
#include <iostream>
#define GLM_FORCE_RADIANS
// #include "tiny_gltf.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

BufferDefs::Layout BasicRenderFlowVk::s_vertexBufferLayout{};

#ifdef PC_DEBUG
constexpr bool showDrawCommandCount = false;
#endif

void BasicRenderFlowVk::CreatePipelines() {

  // CREATE BASE PIPELINE
  // TODO: Create pipeline abstraction in the factory
  //
  // TODO: Add tinyGltf and convert a test scene from blender to Popcorn scene
  // with meshes & materials
  //
  // TODO: Make descriptor sets & resources accordingly

  // TODO: Refactor this to make a pipeline with G-buffer
  // CreateBasePipeline(); // From Pipeline factory
};

void BasicRenderFlowVk::CreateRenderPass() {
  //
  // ----------------------------------------------------------------
  // --- Basic RenderPass -------------------------------------------
  //
  // ATTACHMENT DESCRIPTIONS ----------------------------------------
  VkAttachmentDescription colorAttachment{};
  RenderPassVk::GetDefaultAttachmentDescription(colorAttachment);

  auto *swapchainVk = ContextVk::Swapchain();
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
  RenderPassVk::GetDefaultRenderPassCreateInfo(renderPass1CreateInfo);

  // Dependencies -- For making the renderpass to wait on the color output
  // pipeline stage triggered by "swapchain image available" semaphore
  renderPass1CreateInfo.dependencyCount = 1;
  renderPass1CreateInfo.pDependencies = &dependency;

  renderPass1CreateInfo.pAttachments = attachments.data();
  renderPass1CreateInfo.pSubpasses = subpasses.data();

  //
  // CREATE VULKAN RENDER PASS --------------------------------------
  m_basicRenderPassVk.Create(renderPass1CreateInfo,
                             ContextVk::Device()->GetDevice());
};

void BasicRenderFlowVk::CreateFramebuffers() {
  ContextVk::Swapchain()->CreateSwapchainFramebuffers(
      ContextVk::Device()->GetDevice(), m_basicRenderPassVk.GetVkRenderPass());
};

void BasicRenderFlowVk::RecordRenderCommands(const uint32_t imageIndex,
                                             const uint32_t currentFrame,
                                             VkCommandBuffer &commandBuffer) {
  auto *swapchainVkStn = ContextVk::Swapchain();

  //
  // BEGIN RENDER PASS ---------------------------------------------------------
  VkRenderPassBeginInfo renderPassBeginCreateInfo{};
  m_basicRenderPassVk.GetDefaultCmdBeginRenderPassInfo(
      swapchainVkStn->GetSwapchainFramebuffers()[imageIndex],
      swapchainVkStn->GetSwapchainExtent(), renderPassBeginCreateInfo);
  m_basicRenderPassVk.RecordBeginRenderPassCommand(commandBuffer,
                                                   renderPassBeginCreateInfo);

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
  for (auto &material : m_basicMaterials) {
    // TODO: Choose pipeline according to material
    // BIND PIPELINE -----------------------------------------------------------
    m_colorPipelineVk.RecordBindCmdPipelineCommand(commandBuffer);

#ifdef PC_DEBUG
    if constexpr (showDrawCommandCount) {
      ++drawCommandCount;
    };
#endif

    // TODO: Logic for multiple vertex buffers with VMA

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            m_colorPipelineVk.GetVkPipelineLayout(), 0, 1,
                            &m_globalDescriptorSets[currentFrame], 0, nullptr);

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

      VkPhysicalDeviceProperties deviceProperties;
      vkGetPhysicalDeviceProperties(ContextVk::Device()->GetPhysicalDevice(),
                                    &deviceProperties);

      uint32_t dynamicOffset = i * sizeof(Mesh::Uniforms);

      vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                              m_colorPipelineVk.GetVkPipelineLayout(), 1, 1,
                              &m_localDescriptorSets[currentFrame], 1,
                              &dynamicOffset);

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

void BasicRenderFlowVk::AddMeshToWorkflow(Mesh *mesh) {
  auto ptr = std::find(m_meshes.begin(), m_meshes.end(), mesh);

  if (ptr != m_meshes.end()) {
    PC_WARN("Mesh already exists in m_meshes!")
    return;
  };

  if (mesh->GetVertexBuffer().GetLayout() != s_vertexBufferLayout) {
    PC_ERROR(
        "Mesh vertex layout doesn't match with BasicWorkflow vertex layout",
        "BasicRenderFlow")
    return;
  };

  m_meshes.emplace_back(mesh);
  auto &basicMaterials = mesh->GetMaterialsByType<MaterialTypes::BasicMat>();
  auto &pbrMaterials = mesh->GetMaterialsByType<MaterialTypes::PbrMat>();

  // Each material can potentially have the same material type but different
  // descriptor sets (shaders, textures ..etc)
  for (auto &material : basicMaterials) {
    RegisterMaterial(material);
  }

  for (auto &material : pbrMaterials) {
    RegisterMaterial(material);
  }
}

//
// --------------------------------------------------------------------------
// --- VERTEX BUFFER ALLOCATION ---------------------------------------------
//
void BasicRenderFlowVk::AllocateVkVertexBuffers() {
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

  void *data = BufferVkUtils::MapVkMemoryToCPU(stagingVertexBufferMemory, 0,
                                               currentOffset);

  for (int i = 0; i < m_meshes.size(); ++i) {
    VertexBufferVk &vertexBuffer =
        static_cast<VertexBufferVk &>(m_meshes[i]->GetVertexBuffer());

    // vertexBuffer.PrintBuffer<Vertex>();
    // PC_WARN(vertexBuffer.GetSize())

    BufferVkUtils::CopyBufferCPUToGPU(
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
  auto &device = ContextVk::Device()->GetDevice();
  vkDestroyBuffer(device, stagingVertexBuffer, nullptr);
  vkFreeMemory(device, stagingVertexBufferMemory, nullptr);
};

//
// --------------------------------------------------------------------------
// --- INDEX BUFFER ALLOCATION ----------------------------------------------
//
void BasicRenderFlowVk::AllocateVkIndexBuffers() {
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

    BufferVkUtils::CopyBufferCPUToGPU(
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
  auto &device = ContextVk::Device()->GetDevice();
  vkDestroyBuffer(device, stagingIndexBuffer, nullptr);
  vkFreeMemory(device, stagingIndexBufferMemory, nullptr);
};

//
// --------------------------------------------------------------------------
// --- UNIFORM BUFFERS ALLOCATION -------------------------------------------
//
void BasicRenderFlowVk::AllocateVkUniformBuffers() {
  const VkExtent2D &swapchainExtent =
      ContextVk::Swapchain()->GetSwapchainExtent();
  constexpr auto maxFramesInFlight = RendererVk::MAX_FRAMES_IN_FLIGHT;

  // TODO: Move this to Camera
  // struct GlobalUniform {
  //   glm::mat4 view;
  //   glm::mat4 proj;
  // };

  // View matrix
  m_viewProjUBO.view =
      glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, 1.0f, 0.0f));
  // Projection Matrix
  m_viewProjUBO.proj = glm::perspective(
      glm::radians(45.0f),
      swapchainExtent.width / (float)swapchainExtent.height, 0.1f, 10.0f);
  m_viewProjUBO.proj[1][1] *= -1;

  const VmaAllocator &allocator = MemoryAllocatorVk::Get()->GetVMAAllocator();

  VkDeviceSize meshUBOSize = sizeof(Mesh::Uniforms);
  // PC_WARN("EXPECTED SIZE: " << sizeof(glm::mat4) << ". SIZE: " <<
  // meshUBOSize)

  VkBufferCreateInfo globalBufferInfo = {
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .size = sizeof(m_viewProjUBO),
      .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE};

  VkBufferCreateInfo localBufferInfo = {
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .size = meshUBOSize * m_meshes.size(),
      .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE};

  VmaAllocationCreateInfo allocInfo{};
  allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
  allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

  m_globalUniformBuffers.resize(maxFramesInFlight);
  m_globalUniformAllocations.resize(maxFramesInFlight);
  m_globalUniformAllocationInfos.resize(maxFramesInFlight);
  m_globalMappedUniforms.resize(maxFramesInFlight);

  m_localUniformBuffers.resize(maxFramesInFlight);
  m_localUniformAllocations.resize(maxFramesInFlight);
  m_localUniformAllocationInfos.resize(maxFramesInFlight);
  m_localMappedUniforms.resize(maxFramesInFlight);

  for (size_t i = 0; i < maxFramesInFlight; ++i) {
    // GLOBAL UBOS
    VkResult globalResult = vmaCreateBuffer(
        allocator, &globalBufferInfo, &allocInfo, &m_globalUniformBuffers[i],
        &m_globalUniformAllocations[i], &m_globalUniformAllocationInfos[i]);
    if (globalResult != VK_SUCCESS) {
      std::cerr << "Failed to create global uniform buffer for frame " << i
                << std::endl;
    }
    m_globalMappedUniforms[i] = m_globalUniformAllocationInfos[i].pMappedData;

    // LOCAL UBOS
    VkResult localResult = vmaCreateBuffer(
        allocator, &localBufferInfo, &allocInfo, &m_localUniformBuffers[i],
        &m_localUniformAllocations[i], &m_localUniformAllocationInfos[i]);
    if (localResult != VK_SUCCESS) {
      std::cerr << "Failed to create local uniform buffer for frame " << i
                << std::endl;
    }
    m_localMappedUniforms[i] = m_localUniformAllocationInfos[i].pMappedData;
  }
};

//
// --------------------------------------------------------------------------
// --- DESCRIPTOR POOL & SETS -----------------------------------------------
//

void BasicRenderFlowVk::CreateDescriptorSetLayouts() {
  ContextVk::DescriptorFactory()->CreateDescriptorSetLayouts<Pipelines::Base>();
  // ContextVk::DescriptorFactory()->CreateDescriptorSetLayouts<Pipelines::Deferred>();
};

void BasicRenderFlowVk::CreateDescriptorPool() {
  constexpr uint32_t maxFramesInFlight = RendererVk::MAX_FRAMES_IN_FLIGHT;
  uint32_t maxDSets = 2 * maxFramesInFlight; // 1 global + 1 per-object

  std::vector<VkDescriptorPoolSize> poolSizes = {
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, maxFramesInFlight},
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, maxFramesInFlight},
  };

  VkDescriptorPoolCreateInfo poolInfo{};
  DescriptorPoolVk::GetDefaultDescriptorPoolState(poolInfo, maxDSets,
                                                  poolSizes);
  DescriptorPoolVk::CreateDescriptorPool(poolInfo, &m_descriptorPool);
};

void BasicRenderFlowVk::CreateDescriptorSets() {
  constexpr auto maxFramesInFlight = RendererVk::MAX_FRAMES_IN_FLIGHT;
  auto &device = ContextVk::Device()->GetDevice();

  if (m_descriptorPool == VK_NULL_HANDLE) {
    throw std::runtime_error("Descriptor pool is not initialized!");
  }

  //
  // --- ALLOCATE DESCRIPTOR SETS --------------------------------------
  //
  // Global layouts ----------------------------------------------------
  if (m_globalUBOsDSetLayout == VK_NULL_HANDLE) {
    throw std::runtime_error(
        "Global UBO descriptor set layout is not initialized!");
  }
  VkDescriptorSetAllocateInfo globalDSetsAllocInfo{};
  auto allFramesGlobalLayouts =
      std::vector(maxFramesInFlight, m_globalUBOsDSetLayout);
  DescriptorSetsVk::GetDefaultDescriptorSetAllocateState(
      allFramesGlobalLayouts, m_descriptorPool, globalDSetsAllocInfo);
  DescriptorSetsVk::AllocateDescriptorSets(globalDSetsAllocInfo,
                                           m_globalDescriptorSets);

  //
  // local layouts -----------------------------------------------------
  if (m_localUBOsDSetLayout == VK_NULL_HANDLE) {
    throw std::runtime_error(
        "Local UBO descriptor set layout is not initialized!");
  }
  VkDescriptorSetAllocateInfo localDSetsAllocInfo{};
  auto allFramesLocalLayouts =
      std::vector(maxFramesInFlight, m_localUBOsDSetLayout);
  DescriptorSetsVk::GetDefaultDescriptorSetAllocateState(
      allFramesLocalLayouts, m_descriptorPool, localDSetsAllocInfo);
  DescriptorSetsVk::AllocateDescriptorSets(localDSetsAllocInfo,
                                           m_localDescriptorSets);

  //
  // --- UPDATE DESCRIPTOR SETS ----------------------------------------
  for (size_t i = 0; i < maxFramesInFlight; ++i) {
    //
    // Global buffers --------------------------------------------------
    VkDescriptorBufferInfo globalbufferInfo{};
    globalbufferInfo.buffer = m_globalUniformBuffers[i];
    globalbufferInfo.offset = 0;
    globalbufferInfo.range = sizeof(m_viewProjUBO);

    VkWriteDescriptorSet globalDtrWrite{};
    globalDtrWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    globalDtrWrite.dstSet = m_globalDescriptorSets[i];
    globalDtrWrite.dstBinding = 0;
    globalDtrWrite.dstArrayElement = 0;
    globalDtrWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    globalDtrWrite.descriptorCount = 1;
    globalDtrWrite.pBufferInfo = &globalbufferInfo;
    globalDtrWrite.pImageInfo = nullptr;       // Optional
    globalDtrWrite.pTexelBufferView = nullptr; // Optional

    vkUpdateDescriptorSets(device, 1, &globalDtrWrite, 0, nullptr);

    //
    // Local buffers ----------------------------------------------------
    VkDescriptorBufferInfo localbufferInfo{};
    localbufferInfo.buffer = m_localUniformBuffers[i];
    localbufferInfo.offset = 0;
    localbufferInfo.range = sizeof(Mesh::Uniforms);

    VkWriteDescriptorSet localDtrWrite{};
    localDtrWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    localDtrWrite.dstSet = m_localDescriptorSets[i];
    localDtrWrite.dstBinding = 0;
    localDtrWrite.dstArrayElement = 0;
    localDtrWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    localDtrWrite.descriptorCount = 1;
    localDtrWrite.pBufferInfo = &localbufferInfo;
    localDtrWrite.pImageInfo = nullptr;       // Optional
    localDtrWrite.pTexelBufferView = nullptr; // Optional

    vkUpdateDescriptorSets(device, 1, &localDtrWrite, 0, nullptr);
  }
};

//
//
// --------------------------------------------------------------------------
// --- UPDATE UNIFORMS & PUSH CONSTANTS HERE --------------------------------
// --- RUNS EVERY FRAME -----------------------------------------------------
//

void BasicRenderFlowVk::ProcessSceneUpdates(const uint32_t currentFrame) {
  // Copy view project matrix first
  BufferVkUtils::CopyBufferCPUToGPU(
      (byte_t *)m_globalMappedUniforms[currentFrame], &m_viewProjUBO,
      sizeof(m_viewProjUBO));

  // Then copy each mesh model matrix
  for (size_t i = 0; i < m_meshes.size(); ++i) {
    auto &meshModelMatUBO = m_meshes[i]->GetUniformBuffer();

    BufferVkUtils::CopyBufferCPUToGPU(
        (byte_t *)m_localMappedUniforms[currentFrame] +
            (i * sizeof(meshModelMatUBO)),
        &meshModelMatUBO, sizeof(meshModelMatUBO));
  }
};

//
// --------------------------------------------------------------------------
// --- CLEANUP --------------------------------------------------------------
//
void BasicRenderFlowVk::CleanUp() {
  auto &device = ContextVk::Device()->GetDevice();
  auto *framebuffersVkStn = FramebuffersVk::Get();
  const auto &allocator = MemoryAllocatorVk::Get()->GetVMAAllocator();

  constexpr auto maxFramesInFlight = RendererVk::MAX_FRAMES_IN_FLIGHT;

  // Cleanup uniform buffer memory
  for (size_t i = 0; i < maxFramesInFlight; ++i) {
    vmaDestroyBuffer(allocator, m_globalUniformBuffers[i],
                     m_globalUniformAllocations[i]);
    vmaDestroyBuffer(allocator, m_localUniformBuffers[i],
                     m_localUniformAllocations[i]);
  }

  DescriptorPoolVk::DestroyDescriptorPool(m_descriptorPool);

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
