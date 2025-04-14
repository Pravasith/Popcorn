#pragma once

#include "BufferObjects.h"
#include "Camera.h"
#include "GBufferPipelineVk.h"
#include "GlobalMacros.h"
#include "PipelineFactoryVk.h"
#include "Popcorn/Core/Base.h"
#include "RenderFlowVk.h"
#include "RenderPassVk.h"
#include <cstdint>
#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

struct Uniforms {
  alignas(16) glm::mat4 view;
  alignas(16) glm::mat4 proj;
};

class GBufferRenderFlowVk : public RenderFlowVk {
public:
  GBufferRenderFlowVk()
      : m_pipelineFactory(PipelineFactoryVk::Get()),
        m_gBufferPipeline(
            m_pipelineFactory->GetGfxPipeline<Pipelines::Deferred>()) {

    s_vertexBufferLayout
        .Set<BufferDefs::AttrTypes::Float3, BufferDefs::AttrTypes::Float3>();

    PC_PRINT("CREATED", TagType::Constr, "BasicWorkflowVk")
  };

  virtual ~GBufferRenderFlowVk() override {
    PC_PRINT("DESTROYED", TagType::Destr, "BasicWorkflowVk")
  };

  //
  //
  //
  //
  //
  //
  // NEW -- NEW -- NEW -- NEW -- NEW -- NEW -- NEW -- NEW --
  virtual void CreateAttachments() override;
  //
  //
  //
  //
  //
  //

  [[nodiscard]] inline static const BufferDefs::Layout &
  GetBasicWorkflowVertexLayout() {
    return s_vertexBufferLayout;
  }

  [[nodiscard]] virtual const VkRenderPass &GetRenderPass() const override {
    return m_renderPass.GetVkRenderPass();
  };

  virtual void
  RecordRenderCommands(const uint32_t frameIndex, const uint32_t currentFrame,
                       VkCommandBuffer &currentFrameCommandBuffer) override;

  virtual void ProcessSceneUpdates(const uint32_t currentFrame) override;

  virtual void CreateRenderPass() override;
  virtual void CreatePipelines() override;
  virtual void CreateFramebuffer() override;
  virtual void AllocateVkVertexBuffers() override;
  virtual void AllocateVkIndexBuffers() override;
  virtual void AllocateVkUniformBuffers() override;
  virtual void CreateDescriptorSetLayouts() override;
  virtual void CreateDescriptorPool() override;
  virtual void CreateDescriptorSets() override;

  virtual void CleanUp() override;

private:
  //
  // NEW --------------------------------------------------------------------
  struct ImageViews {
    // G-Buffer image views
    VkImageView colorMap{};
    VkImageView depthMap{};
    VkImageView normalMap{};
  };

  struct FrameBuffer {
    ImageViews imageViews{};
  };

  // Renderpass & 2 subpasses
  RenderPassVk m_renderPass;
  VkSubpassDescription m_subpasses[2]{};

  Camera *camera;

  PipelineFactoryVk *m_pipelineFactory = nullptr;

  // Pipelines
  GBufferPipelineVk &m_gBufferPipeline;

  //
  // OLD --------------------------------------------------------------------

  static BufferDefs::Layout s_vertexBufferLayout;

  std::vector<VkDeviceSize> m_vertexBufferOffsets;
  std::vector<VkDeviceSize> m_indexBufferOffsets;

  VkBuffer m_vkVertexBuffer;
  VkDeviceMemory m_vkVertexBufferMemory;

  VkBuffer m_vkIndexBuffer;
  VkDeviceMemory m_vkIndexBufferMemory;

  std::vector<VkBuffer> m_globalUniformBuffers;
  std::vector<VmaAllocation> m_globalUniformAllocations;
  std::vector<VmaAllocationInfo> m_globalUniformAllocationInfos;
  std::vector<void *> m_globalMappedUniforms;

  std::vector<VkBuffer> m_localUniformBuffers;
  std::vector<VmaAllocation> m_localUniformAllocations;
  std::vector<VmaAllocationInfo> m_localUniformAllocationInfos;
  std::vector<void *> m_localMappedUniforms;

  // TODO: Make this a global
  Uniforms m_viewProjUBO;

  // TODO: Move to global resources (and index into it whenever needed)
  VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;

  VkDescriptorSetLayout m_globalUBOsDSetLayout;
  VkDescriptorSetLayout m_localUBOsDSetLayout;

  std::vector<VkDescriptorSet> m_globalDescriptorSets;
  std::vector<VkDescriptorSet> m_localDescriptorSets;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
